# Firmware spec (QMK keymap repo)

Read `00-protocol.md` first. This describes the keymap as it should be.
Keep the file's existing conventions (`tmux_fkey`, `tmux_mod`,
`tmux_set_mode`, `tmux_switch_mode`, OLED cases, comment style). The sandbox
cannot build or flash: write the code, build in the QMK distrobox, flag
anything that needs a hardware check.

## Layers

`_MAC`, `_LINUX`, `_NUM_*`, `_NAV_*` as they are. tmux layers:

```
_TMUX          shared: mode row + EXIT
_TMUX_TREE
_TMUX_WINDOW
_TMUX_PANE
_TMUX_WITHIN   navigate inside the focused pane's program
```

Exactly one of TREE/WINDOW/PANE/WITHIN is on above `_TMUX` at any time.
There is no APP layer, no per-app toggle and no transcript flag in the
firmware; the program in the pane and its state come from the host.

Mode row on `_TMUX`, transparent on every mode layer: A `TM_TREE`,
S `TM_WIN`, D `TM_PANE`, F `TM_WITHIN`, G unused, T `TM_EXIT`.

## State

Desired state, owned by the keyboard, as today: `tmux_mode`, `tmux_mod`
(RESIZE/SPLIT/MOVE held; WORD/LINE held in WITHIN), base layer.

Context, owned by the host, received over Raw HID:

```c
static struct {
    uint32_t last_ctx_ms;    // host alive iff elapsed < 1500 ms
    uint8_t  os, program, tmux_bits, transcript, phase, perm;
    uint8_t  intent_nonce, intent_status;
    uint8_t  window, pane;
    char     label[17];
    uint8_t  host_seq, my_seq, nonce;
} ctx;
```

`host_alive()` is `timer_elapsed32(ctx.last_ctx_ms) < 1500`, evaluated at
the keypress, never cached.

## Raw HID

- `rules.mk`: `RAW_ENABLE = yes`. With `VIA_ENABLE`, implement in
  `via_command_kb`, since VIA owns `raw_hid_receive`.
- Receive: validate magic, version and type; copy fields into `ctx`; set
  `last_ctx_ms`; apply the CONTEXT rules from the protocol (OS to base layer,
  intent status, contradiction rule); if `ack != my_seq`, resend STATE.
- `send_state(intent, arg)`: builds STATE from `tmux_mode`, `tmux_mod`, base
  layer and the intent; increments `my_seq`; increments `nonce` only when
  `intent != 0`. Called on every change to `tmux_mode` or `tmux_mod`, on
  base-layer change, on init (type HELLO), and from every intent site.
- Split board: HID lands on the master. If the mode OLED is on the slave,
  sync `ctx` and the desired state over a user split transaction so both
  halves render the same. State which half in the PR.

## One path per tmux action

Every tmux action is a single root-table key from the protocol's tmux key
table, sent with `tmux_fkey(KC_F13..KC_F24, mods)`. tmux runs the bound
command immediately, daemon or not. `tmux_mode` and `tmux_mod` update
locally at the same time; the host confirms by observation (the OLED shows
`?` until it does when the host is alive, nothing when it is not).

There is no prefix in the firmware and no second path: no `tmux_key`, no
`tmux_cmd`, no command strings, and no compile-time switch between them. This
is a compiled keymap — a terminal that will not pass F13+ through is a
reflash, not a runtime state to carry. There is likewise no runtime branch on
host liveness for tmux actions.

Program-aware keys are chosen by the keyboard from `ctx` and sent as
keystrokes. Two keys are intents instead, both because Claude's viewer is
unobservable from the keyboard: `TC_DEEP` → WITHIN_DEEP, and entering WITHIN on
a `claude-safe` pane → WITHIN_OPEN.

Intent status, for either: `pending` shows `?`; `done` clears it; `failed`
restores the previous `tmux_mode` and shows `!` for one second. Acted on when
the status changes, not while it holds — the host repeats it every 500 ms until
the next intent, so a level test would pin the mark on. Neither intent actually
moves `tmux_mode`, so the restore is reached by nothing today; it is kept for an
intent that does.

## Layer contents

**TREE, WINDOW, PANE**: as they are, less the three actions the key table has
no key for. PANE's O is unused (no transcript key; WITHIN on a Claude pane
opens the viewer) and so is its M (`TP_BRK`, break-pane). WINDOW loses the
`TW_NEW` thumb and the `new-window -b`/`-a` it modified, which leaves `MOVE`
the only modifier WINDOW has and retires `TMOD_NEW`. Session ←/→ and last
window stay, on `S-F19`/`S-F20`/`S-F21`.

**WITHIN**, right hand:

| Key | Keycode | Meaning |
|---|---|---|
| Y | `TC_NEXT` | next match |
| U | `TC_BKGD` | background the running tool or agent (Claude only) |
| I | `TC_UP` | up |
| O | unused | |
| P | `TC_LEAVE` | leave what WITHIN opened |
| H | `TC_PREV` | previous match |
| J | `TC_LEFT` | previous unit |
| K | `TC_DOWN` | down |
| L | `TC_RGHT` | next unit |
| ; | unused | |
| N, M | unused | |
| , | `TC_COPY` | yank (copy mode only) |
| . | `TC_PSTE` | paste (copy mode only) |
| / | `TC_SRCH` | search |

Left thumbs: outer `TC_WORD`, middle `TC_LINE`, both held; inner unused.
Right thumbs unused. Select/yank positions match the existing COPY layer;
if they differ in the file, keep the file's.

`TC_DEEP` on G while in WITHIN, intent WITHIN_DEEP (copy mode containing
Claude's whole conversation). Decided in and live: no compile switch. Inert
outside Claude, and with no daemon to actuate the intent it does nothing.

## WITHIN resolution

First a target, from `ctx`:

| Condition (checked in order) | Target |
|---|---|
| host not alive, or tmux bit0 clear | `copy` |
| tmux bit2 set (pane in copy mode) | `copy` |
| program = claude and transcript = open | `transcript` |
| program = claude | `claude-safe` |
| program = hunk | `hunk` |
| otherwise | `copy` |

Then the key, with `mod` the held thumb:

| Key, mod | copy | transcript | claude-safe | hunk |
|---|---|---|---|---|
| ↑ ↓, none | as copy-mode today | `KC_UP` / `KC_DOWN` | `KC_UP` / `KC_DOWN` | `KC_UP` / `KC_DOWN` |
| ↑ ↓, WORD | as today | `C(KC_U)` / `C(KC_D)` | `KC_PGUP` / `KC_PGDN` | `u` / `d` |
| ↑ ↓, LINE | as today | `b` / `KC_SPC` | `KC_PGUP` / `KC_PGDN` | `b` / `KC_SPC` |
| ← →, none | as today | `{` / `}` | nothing | `[` / `]` |
| ← →, WORD | as today | `{` / `}` | nothing | `{` / `}` |
| ← →, LINE | as today | `{` / `}` | nothing | `,` / `.` |
| SRCH | `/` | `/` | nothing | `/` |
| NEXT / PREV | `n` / `N` | `n` / `N` | nothing | `n` / `N` |
| LEAVE | F20 (`copy-mode -q`) | `C(KC_O)` | nothing | `KC_ESC` |
| BKGD | nothing | C-F23 | C-F23 | nothing |
| COPY / PSTE | `KC_ENT` / F20 then C-F22 | nothing | nothing | nothing |

Entering WITHIN (`TM_WITHIN` from any mode) resolves the same way: target
`copy` → F19 (`copy-mode`); `claude-safe` → the WITHIN_OPEN intent, and the
OLED shows `?` until the host reports the transcript; `transcript` and `hunk` →
nothing, already navigable. Offline the target is always `copy`, so F19 — which
is why the keyboard never needs to send `Ctrl+o` itself: that path only exists
when the host is alive to run it.

WITHIN_OPEN rather than a keystroke because a busy pane can swallow the key and
only the host can see that and try again. A `failed` status is the viewer
refusing to open; the keyboard stays in `claude-safe`, which is still usable,
rather than dropping out of WITHIN.

Units: in copy mode WORD/LINE are word/line as today; in the transcript the
unit is the prompt; in hunk the ladder is hunk, annotated hunk, file.

Never sent to any program, by construction of the table: `q` (quits hunk),
`Esc` to a Claude prompt (interrupts the turn), `Ctrl+d` to a Claude prompt
(exits Claude Code), `Ctrl+b` anywhere (tmux prefix). `claude-safe` is the
target whenever the viewer is not confirmed open.

`Ctrl+d` is barred from `claude-safe` and not from `transcript`: at the prompt
it exits Claude Code, and in the viewer it is half a page down and the prompt
is not reading. That is what the two targets are for. `Ctrl+b` is barred
everywhere, which is why BKGD is a tmux key — the pane's own `C-x C-b` reaches
Claude through `send-keys`, past whatever the prefix is bound to.

Mode keys from WITHIN: D sends F20 (`copy-mode -q`, which cancels copy or
tree mode and is harmless otherwise) and goes to PANE; it never sends
`Ctrl+o`, so a Claude transcript left open stays open. F again re-resolves
entry, which is a no-op in every target except `claude-safe`.

## OLED

Line 1, host alive: the target word (`COPY`, `TRSC`, `CLAUDE`, `HUNK`) in
WITHIN, else the mode word; then the held modifier as today; then `?` or `!`
for intent status. Host not alive: mode word plus a trailing `~`.
Line 2, host alive and program = claude: phase (`idle`, `run`, `wait`) and
permission mode.

Base layer indicator as today; it follows `ctx.os` on connect unless a
manual override is active.

## Code layout

The wire format is in `kb_protocol.c/.h`: the constants, `kb_state_report`,
`kb_context_parse` and `kb_tmux_event_arg`. It includes no QMK headers and
knows no layer number, keycode or `TMOD_`, so it builds natively —
`test/run.sh` compiles it against `test/kb_protocol_test.c` with nothing but
a C compiler. The keymap translates its own enums into protocol values
(`kb_mode_of`, `kb_mod_of`) and owns the `raw_hid_send`/`receive` wiring, the
rules that touch layers, and the OLED.

Put the target and key resolution in `tmux_context.c/.h` on the same terms:
pure functions of `ctx`, `tmux_mode` and `tmux_mod`, returning a keycode or
an intent, with its own tests under `test/`. The keymap calls them; nothing
else needs to know the tables.

Both are added to the build with `SRC +=` in `rules.mk`.

## Acceptance (QMK distrobox build, then hardware)

- Compiles with `RAW_ENABLE = yes`; every `LAYOUT_split_3x5_3` block has 36
  entries.
- `test/run.sh` passes.
- No daemon: TREE/WINDOW/PANE work with no prefix (each key is one tmux
  root binding); WITHIN is tmux copy mode with WORD/LINE, select, yank,
  search; OLED shows `~`.
- Daemon, shell pane: F enters tmux copy mode; keys are the `copy` column.
- Daemon, Claude pane: F sends the WITHIN_OPEN intent; OLED shows `CLAUDE ?` until the
  host observes the viewer, then `TRSC`; U sends `Ctrl+X Ctrl+B`; ↓ scrolls a line, WORD+↓ half a page,
  ← → step prompts; P closes the viewer and the OLED shows `CLAUDE`.
- Daemon, Claude pane with the viewer closed: ↓ moves the cursor, WORD+↓
  sends PgDn, ← → send nothing, P sends nothing.
- Daemon, hunk pane: F shows `HUNK`; ← → step hunks, WORD+← → annotated
  hunks, LINE+← → files; P sends Esc.
- Cable pulled in WITHIN: within 1.5 s the OLED shows `~`, the next key
  takes the offline path, nothing fires twice on reconnect.
- Base layer follows `os` on connect; a manual toggle is not overridden.
