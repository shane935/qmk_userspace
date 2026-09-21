This is the QMK repo for flashing my keyboard it is a clone of the main QMK repo.

The only code that has any relvence to us is the keyboard setup at: keyboards/crkbd/keymaps/shane935

## Talking about key positions

Use `L(col,row)` / `R(col,row)` for the left and right halves, numbered exactly
as the keys are printed in keymap.c: col 1-5 left to right, row 1-3 top to
bottom. Thumbs are `LT1-3` / `RT1-3`, also in source order.

Right hand columns map to fingers like this (base-layer keys in brackets):

| col | finger              | keys      |
|-----|---------------------|-----------|
| R1  | index, inner stretch| Y / H / N |
| R2  | index               | U / J / M |
| R3  | middle              | I / K / , |
| R4  | ring                | O / L / . |
| R5  | pinky               | P / ; / ' |

So "PgUp on R(5,1)" means the right pinky, top row.

## Layers

`enum layers` is 0-indexed: _MAC(0), _LINUX(1), _NUM_MAC(2), _NUM_LINUX(3),
_NAV_MAC(4), _NAV_LINUX(5). Prefer the names over the numbers.

Every layer is duplicated per OS. A change to one half of a pair almost always
needs the same change to the other, or the two OS modes drift apart. Ask before
assuming a change is meant for only one.
