# Daemon spec (new repo, Node/TypeScript)

Read `00-protocol.md` first. Name is Shane's call; working name `kbd-host`.
Runs on Linux (Bazzite) and macOS. It owns no durable state: on start it
observes tmux, waits for the keyboard's HELLO, and reports.

Follow the repo's dev process (Gherkin -> acceptance tests -> implementation).
The sandbox has no USB and no real tmux session with a terminal, so the
daemon must be built around two injectable transports (HID, tmux) with fakes,
and the acceptance tests drive it through those fakes with exact byte-level
assertions.

## Structure

```
src/
  protocol.ts     encode/decode STATE and CONTEXT (pure, tested)
  hid.ts          HID transport: enumerate by VID/PID + usage page, open,
                  read loop, write, hotplug reopen, last-seen timestamps
  tmux.ts         tmux transport: control-mode client + command runner
  claude.ts       hook script (kbd-claude-hook) + phase mapping
  observe.ts      derives the CONTEXT fields from tmux + claude inputs
  intents.ts      actuates intents one-shot, confirms, reports status
  status.ts       mirrors keyboard STATE into tmux user options
  main.ts         wiring, heartbeat, logging
test/features/   Cucumber
```

## tmux transport

- One control-mode client, attached with the `no-output` flag:
  `tmux -C -f no-output attach -t <session>` (or `-C new`), kept open.
  Parse `%window-pane-changed`, `%pane-mode-changed`, `%session-changed`,
  `%session-window-changed`, `%layout-change`, `%subscription-changed`,
  `%output`. Commands are sent on the same client; results parsed by their
  `%begin`/`%end` blocks.
- Queries use one format string, re-run on every notification:
  `#{session_name} #{window_index} #{window_name} #{pane_index} #{pane_id}
   #{pane_current_command} #{pane_in_mode} #{pane_mode} #{window_zoomed_flag}
   #{@kbd_claude} #{@kbd_perm} #{@kbd_session}`.
- Hooks the daemon installs on connect (idempotent `set-hook -g`), for what
  control mode does not notify: `pane-focus-in`, `client-focus-in`,
  `client-focus-out`, `pane-mode-entered`, `pane-mode-exited`, each running
  `wait-for -S kbd`, which the daemon listens on. `focus-events on`.
- **Subscriptions** (`refresh-client -B`), set once on connect, over all panes
  in the session (`%*`): `claude:%*:#{@kbd_claude}`, `perm:%*:#{@kbd_perm}`,
  `cmd:%*:#{pane_current_command}`. tmux reports changes with
  `%subscription-changed`, at most once a second, so Claude phase and
  permission mode arrive event-driven with up to 1 s latency and no polling.
- **Output watcher**: output is off for every pane by default (the
  `no-output` flag). When the focused pane is a Claude pane the daemon turns
  that one pane on with `refresh-client -A %id:on`, and turns it off again on
  focus change. `%output` for that pane feeds the transcript observer; nothing
  else arrives.
- Program classification: `hunk` -> hunk; `bash|zsh|fish|sh` -> shell; a pane
  with a live Claude session binding -> claude regardless of command name;
  else other.
## Claude Code transport

Claude Code hooks write per-pane tmux user options; the daemon reads them via
the subscriptions above. No HTTP listener.

- Hooks are `command` hooks, all `async: true`, running one script,
  `kbd-claude-hook <event>`, which reads the hook JSON on stdin and runs
  `tmux set -p -t "$TMUX_PANE" @kbd_claude <phase>`, `@kbd_perm
  <permission_mode>`, `@kbd_session <session_id>`. Pane options (`-p`), not
  window options, so two Claude panes in one window do not collide. Outside
  tmux (`$TMUX_PANE` unset) the script exits 0 and does nothing.
- Phase mapping: `SessionStart` -> idle and sets `@kbd_session`;
  `UserPromptSubmit` -> running; `Stop` / `StopFailure` -> idle;
  `PermissionRequest`, `Notification` with `permission_prompt`,
  `idle_prompt`, `agent_needs_input`, `elicitation_dialog` -> waiting;
  `Notification` `agent_completed` -> idle; `SessionEnd` -> unsets all three.
  `permission_mode` from any payload that carries it -> `@kbd_perm`.
- Session binding: a pane whose `@kbd_session` is set is program = claude.
- Ship the hook set as a Claude Code plugin (hooks.json only) so it installs
  without editing settings.json; `kbd-host --print-claude-plugin` emits it.
  The existing `@claude_state` plugins are compatible in spirit but write
  window options and lack permission mode; do not depend on them.
## Transcript observer

- State per Claude pane: `unknown | closed | open`, plus `last_change_ms`.
- Detection is a configurable regex against the **last line** of the pane's
  alternate screen: `capture-pane -p -a -t <pane> -S -1 -E -1` (the `-a` flag
  reads the alternate screen, which is where Claude's viewer draws). Run (a)
  150 ms after the keyboard reports a `Ctrl+o` or after WITHIN_DEEP on a Claude pane,
  (b) when `%output` for that pane has been quiet for 100 ms after a burst,
  (c) every 1 s while the pane is focused and program = claude, as a floor.
  The default regex is a placeholder; Shane fills it from a real
  `capture-pane -a` with the viewer open.
- Anchored to the last line so conversation text never matches.
- Because output is only enabled for the focused Claude pane, (b) never fires
  for other panes; their state goes `unknown` on focus loss.
## Intents

Only WITHIN_DEEP. The keyboard performs every tmux action itself through the
root-table keys in the protocol's tmux key table, and every program-aware key
from CONTEXT; the daemon's job is to observe and report.

- One in flight at a time; a new nonce supersedes (the old one reports
  `failed`).
- WITHIN_DEEP: require focused pane program = claude, transcript observed
  `open`, `pane_in_mode = 0`; else `failed`. Then `send-keys -t <pane> [`,
  poll `#{history_size}` until two samples 100 ms apart are equal (timeout
  2 s → `failed`), then `copy-mode -t <pane>`, `done` on
  `pane-mode-entered`.
- Report `pending` immediately, then `done`/`failed`.

## Observation after keyboard actions

The keyboard tells the host what it just did through STATE (mode changed,
`Ctrl+o` sent), and the host schedules the matching observation:

- mode became WITHIN or TREE, or left either: expect `pane-mode-entered` /
  `pane-mode-exited` within 400 ms; report tmux bits either way.
- `Ctrl+o` sent on a Claude pane: transcript → `unknown` now, run the
  observer at 150 ms and again at 400 ms, report the first definite result.
- pane or window changed: re-run the format query on `%window-pane-changed`
  / `%session-window-changed`; if neither arrives within 400 ms, query anyway.

## Status line mirror

On every STATE: `set -g @kb_mode <word> ; set -g @kb_mod <word> ;
set -g @kb_os <word>` then `refresh-client -S`. Words match the OLED.

## Heartbeat and liveness

- CONTEXT every 500 ms regardless, plus on every change and as a reply to
  every STATE.
- HID hotplug: poll enumeration every 1 s while disconnected; on open, wait
  for HELLO (or send CONTEXT after 500 ms if none), then stream.
- OS byte: `process.platform` -> linux/mac, set once.

## Acceptance scenarios (write as features; each Given builds its own state)

- Protocol: encoding a STATE/CONTEXT with every field at a boundary value
  produces the exact 32 bytes; decoding rejects wrong magic/version.
- A keyboard HELLO after connect receives a CONTEXT with `os` set and
  `ack` equal to the HELLO's seq.
- Focus moving from a shell pane to a hunk pane (fake control-mode
  notifications) produces a CONTEXT with program = hunk, window and pane
  indices, and the window name as label.
- A `%subscription-changed` for `claude` on pane `%3` with value `idle` and
  `session` set, then focus on `%3`, reports program = claude even when
  `pane_current_command` is `node`.
- Subscription values `running`, `idle`, `waiting` map to the phase byte;
  an unset `@kbd_session` makes the pane no longer claude.
- `kbd-claude-hook UserPromptSubmit` with a fixture payload runs exactly
  `set -p -t %3 @kbd_claude running`; with `$TMUX_PANE` unset it runs nothing
  and exits 0; `SessionEnd` unsets all three options.
- Focus moving onto a Claude pane sends `refresh-client -A %id:on`; moving
  off sends `:off`; no other pane is ever turned on.
- A STATE reporting mode WITHIN followed by a fake `pane-mode-entered`
  produces a CONTEXT with tmux bit2 set; with no hook within 400 ms, a
  CONTEXT with bit2 clear.
- A STATE reporting `Ctrl+o` sent on a Claude pane produces transcript
  `unknown` immediately, then `open` when the 150 ms capture matches the
  regex, or `closed` when neither capture matches.
- WITHIN_DEEP with transcript `open` sends `[`, waits for `history_size`
  to settle, sends `copy-mode`, reports done; with transcript `closed` or
  `unknown` reports failed and sends nothing.
- A second intent while one is pending fails the first.
- A `%pane-mode-changed` to not-in-mode with no intent pending is reported
  with status none (the keyboard applies the contradiction rule, not the
  daemon).
- With HID writes failing, the daemon reopens the device on the next
  enumeration and the first message after reopen is a full CONTEXT.
- Every STATE sets the three tmux user options to the matching words.

Unit tests only for `protocol.ts` and the transcript observer's
settle/debounce logic.

## Install (handled by the dotfiles repo, see 30-dotfiles.md)

The daemon exposes `kbd-host --print-udev`, `--print-systemd`,
`--print-launchd`, `--print-claude-plugin`, `--print-tmux-conf` so the config
in dotfiles is generated from one source of truth rather than hand-copied.
`--print-tmux-conf` emits the protocol's tmux key table as root bindings
(`bind -n F13 select-pane -L` …), the `copy-mode-vi` bindings WITHIN relies
on, `focus-events on`, and the status-right fragment. It is also the single
place the F-key → command mapping lives; the firmware's `tmux_fkey` table is
kept in sync by hand against the protocol file.
