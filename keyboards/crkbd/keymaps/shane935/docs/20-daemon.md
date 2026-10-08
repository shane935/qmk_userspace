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
  `permission_mode` from any payload that carries it -> `@kbd_perm`, without
  touching the phase. The hook list here and in `30-dotfiles.md` are one list
  and have to agree.
- The option strings are not the wire values. `observe.ts` maps `@kbd_claude`
  onto CONTEXT byte 9 -- `idle` `1`, `running` `2`, `waiting` `3` -- and sends
  `0` whenever program is not claude, so the OLED's Claude line blanks rather
  than going stale. `@kbd_perm` maps onto byte 10: `default` `1`, `plan` `2`,
  `acceptEdits` `3`, `auto` `4`, `bypassPermissions` `5`, `dontAsk` `6`. A
  string neither list knows is `0` unknown, never the nearest guess -- the OLED
  shows `bypass` before you background something, and a wrong word there is
  worse than no word.
- Session binding: a pane whose `@kbd_session` is set is program = claude.
- Ship the hook set as a Claude Code plugin (hooks.json only) so it installs
  without editing settings.json; `kbd-host --print-claude-plugin` emits it.
  The existing `@claude_state` plugins are compatible in spirit but write
  window options and lack permission mode; do not depend on them.

## Transcript observer

- State per Claude pane: `unknown | closed | open`, plus `last_change_ms`.
- Detection is a configurable regex against the **last line** of the pane's
  alternate screen: `capture-pane -p -a -t <pane> -S -1 -E -1` (the `-a` flag
  reads the alternate screen, which is where Claude's viewer draws). Run (a) as
  WITHIN_OPEN's own confirmation, 150 ms after each `C-o` it sends, which is
  also what decides whether to retry,
  (b) when `%output` for that pane has been quiet for 100 ms after a burst,
  (c) every 1 s while the pane is focused and program = claude, as a floor.
  The default regex is a placeholder; Shane fills it from a real
  `capture-pane -a` with the viewer open.
- Anchored to the last line so conversation text never matches.
- Because output is only enabled for the focused Claude pane, (b) never fires
  for other panes; their state goes `unknown` on focus loss.

## Intents

Two: WITHIN_DEEP (`0x15`) and WITHIN_OPEN (`0x16`). Both are about Claude's
transcript viewer, which the keyboard has no way to observe because Claude draws
on the alternate screen. Everything else the keyboard does itself -- every tmux
action through the root-table keys in the protocol's key table, every
program-aware key resolved from CONTEXT -- and the daemon's job there is only to
observe and report.

- One in flight at a time; a new nonce supersedes (the old one reports
  `failed`). The nonce is what makes an intent one-shot: a repeated STATE
  carrying the same nonce is a resend, not a second request.
- WITHIN_OPEN: require focused pane program = claude and transcript not
  observed `open`; else `failed`. Then `send-keys -t <pane> C-o` and capture
  150 ms later; on no match send it again -- three attempts, 400 ms apart.
  `done` on the first capture that matches, `failed` after the third. This is
  the only intent that retries, and retrying is the whole reason it is one: a
  busy pane swallows the key and the keyboard never learns that it did.
- WITHIN_DEEP: require focused pane program = claude, transcript observed
  `open`, `pane_in_mode = 0`; else `failed`. Then `send-keys -t <pane> [`,
  poll `#{history_size}` until two samples 100 ms apart are equal (timeout
  2 s → `failed`), then `copy-mode -t <pane>`, `done` on
  `pane-mode-entered`.
- Report `pending` immediately, then `done`/`failed`. Neither intent touches the
  keyboard's desired state, so a failure needs nothing undone: both are entered
  from WITHIN and both succeed by changing what the next CONTEXT says.
- Ids `0x10-0x14` and `0x20-0x51` are reserved. An id the daemon does not know
  is logged and reported `failed`, never guessed at.

## Observation after keyboard actions

STATE byte 12 `event` is how the keyboard says what it just did. There is one
event, `1`, "sent a tmux root key", and byte 13 names the row: `1` for F13
through `12` for F24, plus `0x10` Shift, `0x20` Ctrl, `0x40` Alt. **An event is
never actuated.** tmux ran the command the instant the key arrived; the event
exists only so the daemon knows what to watch for, and when.

Decode the row, look it up in the protocol's key table, and schedule the
observation that command implies. Map all of the rows, including the ones marked
"bound, not sent" -- giving one of those a key is meant to be a firmware-only
change, and it stays that way only if the daemon already handles it.

- F19 `copy-mode`, F21 `choose-tree`: expect `pane-mode-entered` within 400 ms;
  report the tmux bits either way.
- F20 `copy-mode -q`: expect `pane-mode-exited` within 400 ms, same.
- F13-F17, S-F13..S-F16, C-F13..C-F16, M-F13..M-F16, S-F17, C-F19: expect
  `%window-pane-changed` or `%layout-change`; if neither arrives within 400 ms,
  re-run the format query anyway.
- F22, F23, F24, S-F19..S-F24, C-F20, C-F21: expect `%session-window-changed`
  or `%session-changed`, same fallback.
- F18 `resize-pane -Z`, S-F18 `next-layout`: expect `%layout-change`, then
  re-read `#{window_zoomed_flag}`.
- C-F17 `detach-client`: the control-mode client goes with it, so reconnect
  rather than observe.
- C-F18 `switch-client -l`: expect `%session-changed`.
- C-F22 `paste-buffer` and C-F23 `send-keys`: nothing to observe, schedule
  nothing.

Desired-state changes do not arrive as events. `mode`, `held modifier` and
`base layer` are fields the daemon diffs against the last STATE it saw.

## Status line mirror

On every STATE: `set -g @kb_mode <word> ; set -g @kb_mod <word> ;
set -g @kb_os <word>` then `refresh-client -S`. Words match the OLED, which for
one value means not reading byte 8 straight: the OLED prints WITHIN's *target*
rather than the mode, so mode `5` becomes `COPY`, `TRSC`, `CLAUDE` or `HUNK` and
the word `WITHIN` never appears. Derive it -- the daemon holds every input that
decision takes, and it is the same resolution the firmware's `within_target`
runs off the same CONTEXT, so in steady state the two agree. TREE, WINDOW and
PANE are the byte. `@kb_mod` is RESIZE, SPLIT, MOVE, WORD or LINE. `@kb_os`
carries `LOCK` beside the OS when byte 10's bit 7 is set.

The daemon decodes reserved values rather than rejecting them, so that a later
VERSION does not need a reader change: byte 8 `1`, byte 9 `4`, and the intent id
ranges above. A reserved or unknown value in a field the daemon only mirrors
leaves the option unset.

## Heartbeat and liveness

- CONTEXT every 500 ms regardless, plus on every change and as a reply to
  every STATE.
- HID hotplug: poll enumeration every 1 s while disconnected; on open, wait
  for HELLO (or send CONTEXT after 500 ms if none), then stream.
- OS byte: `process.platform` -> linux/mac, set once.
- STATE `flags` bit0 is the keyboard's own view of whether the host is alive.
  Log it; if it is clear while the daemon is sending on schedule, log the gap,
  because reports are not arriving and the cable is the suspect.

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
- A STATE with event `1` and arg `0x07` (F19) followed by a fake
  `pane-mode-entered` produces a CONTEXT with tmux bit2 set; with no hook within
  400 ms, a CONTEXT with bit2 clear.
- That same STATE never makes the daemon run `copy-mode` itself. An event is an
  observation schedule, not a command, and the scenario asserts no command was
  sent on the tmux transport.
- Event args decode to the right row under every modifier: `0x16` is S-F18
  `next-layout`, `0x25` is C-F17 `detach-client`, `0x44` is M-F16 `swap-pane`.
- WITHIN_OPEN with transcript `closed` sends `C-o`, and on a capture that
  matches reports done after one attempt; on captures that never match it sends
  three in all, 400 ms apart, then reports failed.
- WITHIN_OPEN on a pane whose transcript is already `open` reports failed and
  sends nothing.
- WITHIN_DEEP with transcript `open` sends `[`, waits for `history_size`
  to settle, sends `copy-mode`, reports done; with transcript `closed` or
  `unknown` reports failed and sends nothing.
- A second intent while one is pending fails the first.
- A STATE with mode `1` or held modifier `4` decodes without error and leaves
  `@kb_mode` / `@kb_mod` unset; an unknown intent id reports failed.
- `@kb_mode` for mode `5` is the derived target word: `COPY` on a shell pane,
  `TRSC` on a Claude pane with the transcript open, `CLAUDE` when it is not,
  `HUNK` on a hunk pane. The word `WITHIN` is never written.
- `--print-tmux-conf` output binds every row of the key table in root and no
  plain key at all; `F20` appears in `copy-mode-vi` and `copy-mode`; `Escape`
  is bound to `clear-selection` in both and to `cancel` in neither.
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
`--print-tmux-conf` is the single place the key → command mapping lives; the
firmware's `tmux_fkey` calls are kept in sync by hand against the protocol file.
It emits:

- every row of the protocol's key table as a root binding
  (`bind -n F13 select-pane -L` …), including the rows marked "bound, not
  sent", so giving one a key later is a firmware-only change;
- F20 again in `copy-mode-vi` and in `copy-mode`, both to `copy-mode -q`;
- the WITHIN copy-mode keys in **both** mode tables, because `mode-keys` decides
  which one a pane in copy mode uses, and a key bound in only one works for a vi
  user and not an emacs one. `Escape` is `clear-selection`, not `cancel` --
  leaving copy mode is a root-table key, so Escape is free to do the thing
  nothing else can;
- `set -g focus-events on` and the status-right fragment.

It must **not** bind any plain key in the root table. tmux's default root table
holds nothing but mouse bindings, and `choose-tree` sets no mode key table, so a
pane in it stays on root and reaches the tree's own keys only because nothing
there matches them first. A `bind -n x` or `bind -n Enter` would take those keys
from choose-tree, and the keyboard's TREE layer with them.
