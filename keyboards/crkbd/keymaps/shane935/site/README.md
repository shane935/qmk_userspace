# Keymap reference page

A single-file reference for this keymap, generated from `keymap.c`.

```sh
node site/build.mjs            # regenerate index.html
node site/build.mjs --check    # exit 1 if index.html is stale
node site/build.mjs --dump     # print the parsed data as JSON
```

Then open `site/index.html` in a browser. It is self-contained — no server, no
`npm install`, no network. Nothing here is reachable by a QMK build.

`index.html` is **not committed**: it is ~4k lines of mostly inlined JSON, and
committing it swamped the diff every time the keymap changed. Build it after
cloning, and rebuild it after editing `keymap.c`.

## What is generated and what is written by hand

The `keymaps[]` array is parsed, so the key grid cannot drift. Everything the
array *encodes* is decomposed automatically: `LCTL_T(KC_S)` becomes tap-S /
hold-Control, `_______` is resolved to the key it actually falls through to,
and the Mac-versus-Linux differences are computed by diffing the layer pairs.

What the array does **not** encode is written by hand in `annotations.mjs`:
`OS_SWAP` (it lives in `process_record_user`), the both-shifts Caps Word chord
(it lives in `config.h` and has no array entry at all), the 200 ms tapping
term, and the Tmux layer being a toggle rather than a hold. Edit that file when
the behaviour changes.

## Files

| file | |
|---|---|
| `parse.mjs` | `keymap.c` → a grid of keycode tokens |
| `keycodes.mjs` | closed token → label table |
| `decorate.mjs` | tokens → key objects; transparency, OS diff, access paths |
| `annotations.mjs` | hand-written prose — **edit this** |
| `template.html` | markup, CSS and page JS — **edit this** |
| `index.html` | generated, not committed — do not edit |

## Keeping it honest

Three things stop the page quietly going stale:

1. **The keycode table is closed.** A keycode not in `keycodes.mjs` fails the
   build rather than rendering a blank cap.
2. **Annotation coverage is enforced.** Any custom keycode, layer toggle or
   space-cadet key without a `keyNotes` entry fails the build, as does a note
   pointing at a key that no longer exists.
3. **`--check`** byte-compares your built `index.html` against a fresh build, so
   it catches "I edited the keymap and forgot to rebuild". Output is
   deterministic — there is no timestamp — so this is reliable.

The footer prints the short hash of `keymap.c` the page was built from, so
`sha256sum keymap.c` tells you whether it is current.
