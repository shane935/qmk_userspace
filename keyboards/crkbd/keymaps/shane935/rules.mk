OLED_ENABLE = yes
CAPS_WORD_ENABLE = yes
RAW_ENABLE = yes

# The wire format, kept out of keymap.c so it can be built and tested without
# QMK. test/run.sh does exactly that.
SRC += kb_protocol.c
SRC += tmux_context.c
SRC += kb_rules.c
