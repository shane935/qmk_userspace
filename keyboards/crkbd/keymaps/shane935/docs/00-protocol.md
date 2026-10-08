# Context-aware keyboard: shared protocol

This file is the contract between the three implementations. It is edited here,
in the firmware repo, beside `20-daemon.md` and `30-dotfiles.md`; the daemon and
dotfiles repos take copies. Changes go in here first and are copied out, never
the other way round.

`VERSION` tracks the wire format and the key table — the things one
implementation can disagree with another about. Correcting the prose to describe
what the implementations already do is not a change in that sense and does not
bump it.

## Model

- The **keyboard** is the single authority for *desired state*: which tmux
  mode it is in, which modifier is held, which base layer it is on. It sends
  that state, plus any intent that just changed it, to the host.
- The **host** is the authority for exactly one thing: the OS it runs on. It
  also *reports* context it can observe (focused window/pane, program in the
  pane, copy mode, transcript, Claude phase) and the status of the last intent.
  Reports are inputs to the keyboard's own reducer, never commands.
- An intent is **one-shot**: the host actuates, observes, retries a bounded
  number of times, reports `done` or `failed`, and stops. It never enforces.
- A report that contradicts desired state while no intent is pending means the
  world moved (a pane closed, a program exited); the keyboard adopts it. This is
  a safety net, not a normal path.
- **tmux actions are keys, not intents.** Every tmux action the keyboard
  performs is a single prefix-free key (F13–F24 and, where the terminal
  reports them distinctly, their modified forms) bound in tmux's root key
  table to the command. tmux runs the command the instant the key arrives,
  with or without the daemon, so there is one path per key and nothing to
  confirm beyond observing the result. The firmware does not know the prefix
  and holds no second path: a terminal that will not pass these keys through
  is a terminal this keyboard does not drive.
- **Intents** exist only for actions that need the host to wait on, or retry
  against, something the keyboard cannot see: WITHIN_DEEP and WITHIN_OPEN.
  Both are about Claude's transcript viewer, which the keyboard has no way to
  observe — Claude draws on the alternate screen. Everything else the keyboard
  decides itself from CONTEXT and sends as keys (`copy-mode` on a shell pane,
  the vi motions inside it); the host's job there is only to observe and
  report, and the OLED shows `?` until the observation lands.
- An intent is used rather than a keystroke only where the host can do
  something the keyboard cannot. Opening the viewer qualifies because the key
  can be swallowed by a busy pane and only the host can tell and try again;
  it is not a tmux action, and the keyboard never sends `Ctrl+o` itself.
- **Offline** (no fresh host report) the keyboard has no program knowledge,
  so WITHIN is tmux copy mode and nothing else; all tmux keys still work.

## Transport

QMK Raw HID (ZMK: zzeneg/zmk-raw-hid, same constants). Usage page `0xFF60`,
usage `0x61`. Every report in both directions is exactly 32 bytes, zero
padded. Little-endian. The keyboard's VID/PID come from its `info.json`.

Liveness: the host sends a CONTEXT report at least every 500 ms, whether or
not anything changed. The keyboard treats the host as **alive** only if the
last CONTEXT arrived within the last 1500 ms. Alive is checked at the moment
a key is handled; it is never cached across keypresses.

Every message carries the sender's `seq` and the last `seq` it saw from the
peer (`ack`). A reply whose `ack` is not the sender's current `seq` answered an
older message; the sender resends its current state. No request ids, no
other acks.

```
VERSION = 2
```

## Keyboard -> host: STATE

Sent on every change to an owned field, on every intent, on connect, and as a
reply to every CONTEXT whose `ack` is stale.

| Byte | Field | Values |
|---|---|---|
| 0 | magic | `0xA5` |
| 1 | version | `VERSION` |
| 2 | seq | uint8, wraps |
| 3 | ack | last host seq seen, `0xFF` = none |
| 4 | type | `0x01` HELLO (first message after connect), `0x02` STATE |
| 5 | intent | `0x00` none, else an intent id below |
| 6 | intent arg | per intent |
| 7 | intent nonce | uint8, increments per intent; host echoes it |
| 8 | mode | `0` tmux layer off, `1` reserved (the TMUX base is never the top mode layer, so nothing sends it), `2` TREE, `3` WINDOW, `4` PANE, `5` WITHIN |
| 9 | held modifier | `0` none, `1` RESIZE, `2` SPLIT, `3` MOVE, `4` reserved, `5` WORD, `6` LINE |
| 10 | base layer | `0` unknown, `1` linux, `2` mac; bit 7 set = manual override active |
| 11 | flags | bit0 = keyboard currently considers host alive |
| 12 | event | what the keyboard just did, so the host can schedule its observation: `0` none, `1` sent a tmux root key |
| 13 | event arg | for event `1`: the tmux key table row (`1` = F13 … `12` = F24, +`0x10` Shift, +`0x20` Ctrl, +`0x40` Alt) |
| 14..31 | zero | |

### Intent ids

| Id | Name | Arg | Host action |
|---|---|---|---|
| `0x15` | WITHIN_DEEP | | focused pane is Claude with transcript observed open and not in copy mode: `send-keys [`, wait for the pane's `history_size` to stop growing, then `copy-mode`; otherwise `failed` |
| `0x16` | WITHIN_OPEN | | focused pane is Claude with the transcript not observed open: `send-keys C-o`, observe whether the viewer opened, retry a bounded number of times, then `done` or `failed` |

These are the only two. Everything else is a root-table tmux key (see the key
table below) or a keystroke the keyboard chooses from CONTEXT. New intents take
the next free id. `0x10–0x14`, `0x20–0x51` and `4` in byte 9 are reserved: do
not assign them, so that an implementation reading either field does not have to
care which version it is talking to.

## tmux key table (root bindings, generated by the daemon, used by firmware)

The keyboard sends the key; tmux's root table runs the command. The daemon's
`--print-tmux-conf` emits exactly this table; the firmware's `tmux_fkey()`
sends exactly these keys and knows nothing else. Every command lives here and
only here, so a flag changes in the generated conf and the firmware does not
move. The terminal has to report F13–F24 and their Shift, Ctrl and Alt forms
distinctly (see dotfiles spec); there is no fallback if it does not.

| Key | Command | Used by |
|---|---|---|
| F13 | `select-pane -L` | PANE ← |
| F14 | `select-pane -D` | PANE ↓ |
| F15 | `select-pane -U` | PANE ↑ |
| F16 | `select-pane -R` | PANE → |
| F17 | `select-pane -l` | PANE last |
| F18 | `resize-pane -Z` | PANE zoom |
| F19 | `copy-mode` | WITHIN entry on a non-Claude, non-hunk pane |
| F20 | `copy-mode -q` | leave any mode; also bound in the mode tables, see below |
| F21 | `choose-tree -Zw -O activity` | TREE entry |
| F22 | `select-window -p` | WINDOW ← |
| F23 | `select-window -n` | WINDOW → |
| F24 | `new-window -c '#{pane_current_path}'` | bound, not sent |
| S-F13..S-F16 | `resize-pane -{L,D,U,R} 5` | PANE RESIZE + arrow |
| C-F13..C-F16 | `split-window -{hb,v,vb,h} -c '#{pane_current_path}'` | PANE SPLIT + arrow |
| M-F13..M-F16 | `swap-pane -s '{left-of}'` / `'{down-of}'` / `'{up-of}'` / `'{right-of}'` | PANE MOVE + arrow |
| S-F17 | `kill-pane` | bound, not sent |
| S-F18 | `next-layout` | PANE layout |
| S-F19 | `switch-client -p` | WINDOW session ← |
| S-F20 | `switch-client -n` | WINDOW session → |
| S-F21 | `select-window -l` | WINDOW last |
| S-F22 / S-F23 | `swap-window -d -t -1` / `-d -t +1` | WINDOW MOVE |
| S-F24 | `kill-window` | bound, not sent |
| C-F17 | `detach-client` | DETACH |
| C-F18 | `switch-client -l` | bound, not sent |
| C-F19 | `break-pane` | bound, not sent |
| C-F20 | `new-window -b -c '#{pane_current_path}'` | bound, not sent |
| C-F21 | `new-window -a -c '#{pane_current_path}'` | bound, not sent |
| C-F22 | `paste-buffer` | PSTE |
| C-F23 | `send-keys C-x C-b` | WITHIN BKGD on a Claude pane |

"Bound, not sent" means the daemon emits the binding and no key on the keyboard
reaches it. They are in the table so that giving one a key is a firmware-only
change rather than a `VERSION` bump. Killing lives in the tree, where it takes
two presses and a cursor already on the thing; break-pane and the two
new-window variants are wanted but have no key yet.

The flags are part of the contract, not decoration, because the firmware has no
other way to ask for them: `swap-window -d` is what makes the client follow the
window it just moved, `-c '#{pane_current_path}'` is what opens a new pane or
window beside the one it came from rather than in `$HOME`, and `swap-pane`
takes the neighbour as `-s` rather than `-t` so focus ends up on the pane that
moved, which is what lets repeated presses push one pane along a row.

`C-F23` is the odd one: the command it runs sends keys to the program rather
than doing anything to tmux. Claude's background chord is `C-x C-b`, and `C-b`
is tmux's default prefix, so a keyboard typing it directly would have tmux
swallow the second half and treat the next key as a prefix command. Routing it
through `send-keys` writes straight into the pane, past tmux's key tables, so
it works whatever the prefix is bound to. That is the only reason the firmware
never emits `C-b` for anything.

F20 is bound in `copy-mode-vi` and `copy-mode` as well as in the root table,
to the same `copy-mode -q`. It is the only row of this table the keyboard sends
while a pane is already in a mode — every other row either moves the pane into a
mode or is sent once it is out of one.

The two mode-table bindings are belt and braces. A pane in copy mode has its key
looked up in `copy-mode-vi` or `copy-mode`, and on a miss tmux retries it in the
root table, so the root binding would fire on its own; binding it in all three
costs two lines and makes "leave whatever mode this pane is in" true without
resting on that fallback. There are two mode tables rather than one because
`mode-keys` decides which of them a pane in copy mode uses, so a key WITHIN
relies on has to be in both or it works for a vi user and not an emacs one.

`choose-tree` neither uses those tables nor needs to. It sets no mode key table,
so a pane in it is still on the root table and F20 leaves it through the same
root binding as everything else. The keys the keyboard sends into it — the four
arrows, `M--` and `M-+` to collapse and expand everything, `x` to kill whatever
is highlighted and `y` to answer the prompt that raises, and `Enter` to choose —
are not bindings in any table at all: tmux handles them inside the mode, which is
where a key goes when it matches nothing in root. The default root table holds
only mouse bindings, so nothing shadows them, and the generated conf must keep it
that way — a plain key bound in root would be taken from `choose-tree`.

Inside copy mode the keyboard sends copy-mode-vi's own keys, and the generated
conf binds the ones WITHIN uses explicitly so they do not depend on `mode-keys`:
the four arrows, `PageUp`/`PageDown`, `b`/`w` for a word, `0`/`$` for a line,
`/` `n` `N` for search, `Space` to begin a selection and `V` to select whole
lines, `Escape` for `clear-selection`, and `Enter` for `copy-pipe-and-cancel`.
That is the whole copy-mode set; leaving copy mode and pasting are root-table
keys, not copy-mode-vi ones, which is why `Escape` is free to clear rather than
cancel.

## Host -> keyboard: CONTEXT

Sent on every observed change, on every intent status change, as the reply to
every STATE, and at least every 500 ms.

| Byte | Field | Values |
|---|---|---|
| 0 | magic | `0xA5` |
| 1 | version | `VERSION` |
| 2 | seq | uint8, wraps |
| 3 | ack | last keyboard seq seen, `0xFF` = none |
| 4 | type | `0x81` CONTEXT |
| 5 | os | `0` unknown, `1` linux, `2` mac. **Authoritative.** |
| 6 | program | `0` unknown, `1` shell, `2` claude, `3` hunk, `4` other |
| 7 | tmux bits | bit0 tmux present, bit1 client focused, bit2 pane in copy mode, bit3 pane in another mode (tree/choose), bit4 window zoomed |
| 8 | transcript | `0` unknown, `1` closed, `2` open. Only meaningful when program = claude |
| 9 | claude phase | `0` none, `1` idle, `2` running, `3` waiting (permission or input) |
| 10 | claude permission mode | `0` unknown, `1` default, `2` plan, `3` acceptEdits, `4` auto, `5` bypassPermissions, `6` dontAsk |
| 11 | intent nonce | the nonce this status refers to |
| 12 | intent status | `0` none, `1` pending, `2` done, `3` failed |
| 13 | window index | |
| 14 | pane index | |
| 15..30 | label | 16 bytes, window name, NUL padded, ASCII only |
| 31 | zero | |

A CONTEXT with `program = 0` and tmux bit0 clear means "no tmux session
focused"; the keyboard treats it as offline for the purpose of intents (it
emits keystrokes) but still applies `os`.

## How the keyboard uses CONTEXT

- `os`: applied to the base layer on the first CONTEXT after connect unless a
  manual override is active. Later changes are applied only if no override.
- `program` + `transcript` + tmux bits: the lookup key for what WITHIN-mode
  navigation keys send. The firmware's `tmux_context.c` is the table.
- intent status: `pending` marks the OLED, `done` clears the mark, `failed`
  marks it differently for a second. Desired state is not touched by any of
  them: both intents are entered from WITHIN and both succeed by changing what
  the host reports, so there is nothing for a failure to undo.
- transcript anything but `open` while program = claude: the `claude-safe` key
  set, which is what `closed` gets as well as `unknown`. Only `open` leaves it.
  `unknown` additionally shows `?`, because there the keyboard is still waiting
  to hear rather than being told the viewer is shut.
- contradiction rule: a CONTEXT showing no mode open while desired is TREE or
  WITHIN (copy), with intent status `none`, sets desired to PANE.

## How the host uses STATE

- `mode`, `held modifier`, `base layer`: mirrored into tmux user options for
  the status line (`@kb_mode`, `@kb_mod`, `@kb_os`), nothing else.
- `intent`: actuated once per nonce. A repeated STATE with the same nonce is a
  resend, not a new intent.
- `event`: never actuated (the keyboard already did it); it schedules the
  observation described in the daemon spec and is otherwise ignored.
- `flags` bit0: logged; if the keyboard believes the host dead while the host
  is sending, the host logs the gap (it indicates a flaky cable).
