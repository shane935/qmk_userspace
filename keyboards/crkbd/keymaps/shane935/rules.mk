OLED_ENABLE = yes
CAPS_WORD_ENABLE = yes
RAW_ENABLE = yes

# The atmega32u4 has 28672 bytes and Raw HID wants its share, so the features
# this keymap never uses are turned off rather than left on by inheritance.
# keyboards/crkbd/info.json switches these on for the board; nothing here sends
# a media key, a mouse key or an RGB keycode.
LTO_ENABLE = yes
EXTRAKEY_ENABLE = no
NKRO_ENABLE = no
MOUSEKEY_ENABLE = no
RGB_MATRIX_ENABLE = no
RGBLIGHT_ENABLE = no
# No hold-a-key-at-plug-in route into the bootloader: use the reset button.
BOOTMAGIC_ENABLE = no

# The wire format, kept out of keymap.c so it can be built and tested without
# QMK. test/run.sh does exactly that.
SRC += kb_protocol.c
SRC += tmux_context.c
SRC += kb_rules.c
