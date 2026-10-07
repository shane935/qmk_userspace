# Checks that need a keyboard

`test/run.sh` covers the pure parts. Everything below needs a flashed board, and
most of it needs the daemon, so it is a list to work through rather than
something that runs.

## Build

- Compiles with `RAW_ENABLE = yes`; every `LAYOUT_split_3x5_3` block has 36
  entries. `site/build.mjs` enforces the second one on every build.
- `test/run.sh` passes.

## No daemon

- TREE, WINDOW and PANE work with no prefix — each key is one tmux root
  binding, so this also checks the generated conf is loaded.
- WITHIN is tmux copy mode: WORD/LINE steps, select, yank, search.
- OLED shows a trailing `~`.

## With the daemon

- Shell pane: F enters copy mode and the keys are the `copy` column.
- Claude pane, viewer shut: F shows `CLAUDE ?` until the host reports the viewer
  open, then `TRSC`. ↓ scrolls a line, WORD+↓ half a page, ← → step prompts.
  P closes the viewer and the OLED goes back to `CLAUDE`.
- Claude pane, viewer open: F again shows `?`, then `COPY` once the host has
  dumped the conversation and opened copy mode. The vi keys, search and yank
  then work over the whole conversation rather than one frame.
- Claude pane with no viewer: ↓ moves the cursor, WORD+↓ sends PgDn, ← → send
  nothing, P sends nothing. Nothing types a letter into the message box.
- U sends `Ctrl+X Ctrl+B` on both Claude targets — the one key that means the
  same in each — and tmux does not swallow the `Ctrl+B`.
- hunk pane: F shows `HUNK`; ← → step hunks, WORD+← → annotated hunks,
  LINE+← → files; P sends Esc and never `q`.
- Base layer follows `os` on connect. OS_SWAP cycles follow, Mac, Linux, follow,
  and shows `LOCK` while it is overruling the host.
- Claude phase and permission mode appear on the third OLED line for a Claude
  pane and nowhere else.
- A failed intent shows `!` for one second and then stops showing it. This has
  never run: both intents need the daemon, and the status handling was wrong in
  two ways the first time, so it is worth testing before it is trusted.

## Known rough edges

- Cable pulled while in WITHIN: within 1.5 s the OLED shows `~` and the next key
  takes the offline path. But nothing re-runs the entry, so copy mode is not
  actually open, and the copy column's letters — `b`, `w`, `0`, `$`, `n`, `V` —
  go to whatever program is in the pane. `q` cannot be sent, so hunk cannot be
  quit, but the rest will do whatever that program does with them. Re-running
  entry when the host disappears would fix it.
- Copy mode has no key for a character-wise selection (`TC_SEL` is `V`, whole
  lines) and none for clearing a selection without leaving. `;`, N and M are
  free on WITHIN for either. Clearing also needs a `clear-selection` binding in
  the generated conf, which binds `Escape` to `cancel`.
- If the daemon misreports the program, WITHIN falls back to `copy` and opens
  tmux copy mode over a full-screen TUI, which shows one frame and scrolls
  nowhere. Useless rather than harmful, but the OLED will say `COPY` and the
  keys will be going to tmux.
