#include QMK_KEYBOARD_H

enum layers {
    _MAC,
    _LINUX,
    _NUM_MAC,
    _NUM_LINUX,
    _NAV_MAC,
    _NAV_LINUX,
    // tmux mode. _TMUX is the shared base and is on whenever tmux mode is on,
    // with exactly one of the mode layers above it. All of them are shared by
    // both OS modes, because the tmux prefix is Ctrl on Mac and Linux alike.
    _TMUX,
    _TMUX_TREE,
    _TMUX_WINDOW,
    _TMUX_PANE,
    _TMUX_COPY,
};

enum custom_keycodes {
    OS_SWAP = SAFE_RANGE,
    // Mode switching, shared by every tmux layer.
    TMUX_ON,
    TM_TREE,
    TM_WIN,
    TM_PANE,
    TM_COPY,
    TM_EXIT,
    TM_DTCH,
    // PANE mode.
    TP_UP,
    TP_DOWN,
    TP_LEFT,
    TP_RGHT,
    TP_LAST,
    TP_ZOOM,
    TP_LYT,
    TP_BRK,
    TP_RSZE,
    TP_SPLT,
    TP_MOVE,
    TP_JOIN,
    TP_PICK,
    // WINDOW mode.
    TW_UP,
    TW_DOWN,
    TW_LEFT,
    TW_RGHT,
    TW_LAST,
    TW_NEW,
    TW_MOVE,
    TW_RNS,
    TW_SEND,
    TW_PICK,
    // TREE mode.
    TT_SEL,
    TT_CLSE,
    TT_FLTR,
    // COPY mode.
    TC_UP,
    TC_DOWN,
    TC_LEFT,
    TC_RGHT,
    TC_COPY,
    TC_PSTE,
    TC_WORD,
    TC_LINE,
    TC_SRCH,
};

// Thumbs: space = nav, enter = numbers
#define M_SPC LT(_NAV_MAC, KC_SPC)
#define L_SPC LT(_NAV_LINUX, KC_SPC)
#define M_ENT LT(_NUM_MAC, KC_ENT)
#define L_ENT LT(_NUM_LINUX, KC_ENT)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MAC] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
             KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,                                        KC_Y,         KC_U,         KC_I,         KC_O,         KC_P,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_A, LCTL_T(KC_S), LALT_T(KC_D), LGUI_T(KC_F),         KC_G,                                        KC_H, RGUI_T(KC_J), RALT_T(KC_K), RCTL_T(KC_L),      KC_SCLN,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,                                        KC_N,         KC_M,      KC_COMM,       KC_DOT,      KC_QUOT,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    KC_LBRC,        M_SPC,      SC_LSPO,         SC_RSPC,        M_ENT,       KC_RBRC
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_LINUX] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
             KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,                                        KC_Y,         KC_U,         KC_I,         KC_O,         KC_P,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_A, LGUI_T(KC_S), LALT_T(KC_D), LCTL_T(KC_F),         KC_G,                                        KC_H, RCTL_T(KC_J), RALT_T(KC_K), RGUI_T(KC_L),      KC_SCLN,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,                                        KC_N,         KC_M,      KC_COMM,       KC_DOT,      KC_QUOT,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    KC_LBRC,        L_SPC,      SC_LSPO,         SC_RSPC,        L_ENT,       KC_RBRC
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_NUM_MAC] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
             KC_7,         KC_8,         KC_9,         KC_0,       KC_GRV,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_4,         KC_5,         KC_6,      KC_MINS,      KC_BSLS,                                     KC_RSFT,      KC_RGUI,      KC_RALT,      KC_RCTL,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_1,         KC_2,         KC_3,       KC_EQL,      KC_SLSH,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      KC_LSFT,         XXXXXXX,      _______,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_NUM_LINUX] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
             KC_7,         KC_8,         KC_9,         KC_0,       KC_GRV,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_4,         KC_5,         KC_6,      KC_MINS,      KC_BSLS,                                     KC_RSFT,      KC_RCTL,      KC_RALT,      KC_RGUI,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
             KC_1,         KC_2,         KC_3,       KC_EQL,      KC_SLSH,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      KC_LSFT,         XXXXXXX,      _______,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_NAV_MAC] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          OS_SWAP,      XXXXXXX,      XXXXXXX,      XXXXXXX,      TMUX_ON,                                      KC_TAB,      KC_BSPC,        KC_UP,       KC_DEL,       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      KC_LCTL,      KC_LALT,      KC_LGUI,      KC_LSFT,                                   G(KC_TAB),      KC_LEFT,      KC_DOWN,      KC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     G(KC_Z),      G(KC_X),      G(KC_C),      G(KC_V),      G(KC_F),
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      _______,      XXXXXXX,         KC_RSFT,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_NAV_LINUX] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          OS_SWAP,      XXXXXXX,      XXXXXXX,      XXXXXXX,      TMUX_ON,                                      KC_TAB,      KC_BSPC,        KC_UP,       KC_DEL,       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      KC_LGUI,      KC_LALT,      KC_LCTL,      KC_LSFT,                                   C(KC_TAB),      KC_LEFT,      KC_DOWN,      KC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     KC_UNDO,       KC_CUT,      KC_COPY,     KC_PASTE,      KC_FIND,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      _______,      XXXXXXX,         KC_RSFT,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    // The mode keys and EXIT live here alone, so every mode layer leaves them
    // transparent and they work identically wherever you are.
    [_TMUX] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      TM_EXIT,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          TM_TREE,       TM_WIN,      TM_PANE,      TM_COPY,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      XXXXXXX,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    // Tree mode drives choose-tree's own key table, so most of it is plain keys
    // sent straight through. Alt -/+ are fold all / unfold all; bare -/+ are
    // only aliases for Left/Right, which J and L already cover.
    [_TMUX_TREE] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     XXXXXXX,   A(KC_MINS),        KC_UP,   A(KC_PLUS),      TT_CLSE,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      XXXXXXX,                                     XXXXXXX,      KC_LEFT,      KC_DOWN,      KC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      TT_FLTR,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      XXXXXXX,         XXXXXXX,       TT_SEL,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_TMUX_WINDOW] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     TW_LAST,       TW_RNS,        TW_UP,      XXXXXXX,      TM_DTCH,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      XXXXXXX,                                     XXXXXXX,      TW_LEFT,      TW_DOWN,      TW_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      TW_SEND,      XXXXXXX,      XXXXXXX,      TW_PICK,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,       TW_NEW,      TW_MOVE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_TMUX_PANE] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     TP_LAST,      TP_ZOOM,        TP_UP,       TP_LYT,      TM_DTCH,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      XXXXXXX,                                     XXXXXXX,      TP_LEFT,      TP_DOWN,      TP_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,       TP_BRK,      XXXXXXX,      TP_JOIN,      TP_PICK,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    TP_RSZE,      TP_SPLT,      TP_MOVE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    // Copy mode drives tmux's copy-mode-vi key table, so the unmodified keys go
    // straight through as the vi keys they are.
    [_TMUX_COPY] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                        KC_N,       KC_SPC,        TC_UP,      S(KC_V),       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      XXXXXXX,                                     S(KC_N),      TC_LEFT,      TC_DOWN,      TC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      TC_COPY,      TC_PSTE,      TC_SRCH,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      TC_WORD,      TC_LINE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  )
};
// clang-format on

#define TMUX_PREFIX C(KC_B)

// Which mode layer sits on top of _TMUX, or TMUX_OFF when tmux mode is off.
// Layer 0 is _MAC and so can never be a mode, which leaves it free to mean off.
#define TMUX_OFF 0
static uint8_t tmux_mode = TMUX_OFF;

// The left thumb modifier in effect. PANE and WINDOW hold theirs, COPY toggles
// its own, and only ever one at a time. PANE and WINDOW both call their inner
// thumb MOVE and neither can be on while the other is, so they share a value.
enum tmux_modifier {
    TMOD_NONE,
    TMOD_RESIZE,
    TMOD_SPLIT,
    TMOD_MOVE,
    TMOD_NEW,
    TMOD_WORD,
    TMOD_LINE,
};
static uint8_t tmux_mod = TMOD_NONE;

// Some keys open a tmux prompt, which needs the keyboard back for as long as it
// is up. The tmux layers come off and the mode waits here until it is over.
enum tmux_prompt_state {
    TPROMPT_NONE,
    TPROMPT_TYPING,
    // The key that opened a one-key prompt has not been released yet, and its
    // own release is not the keypress the prompt is waiting for.
    TPROMPT_ARMING,
    TPROMPT_ONEKEY,
};
static uint8_t tmux_prompt = TPROMPT_NONE;
static uint8_t tmux_prompt_mode = TMUX_OFF;
// The layer a one-key prompt turned on to reach the digits, or 0 for none.
// Layer 0 is _MAC, which is never turned on this way.
static uint8_t tmux_prompt_layer = 0;

// New panes and windows open where the current pane is.
#define TMUX_CWD " -c '#{pane_current_path}'"

// Every tmux key the keyboard sends is the prefix followed by one key.
static void tmux_key(uint16_t keycode) {
    tap_code16(TMUX_PREFIX);
    tap_code16(keycode);
}

// Anything without a suitable default binding goes through tmux's command
// prompt, so none of this depends on the user's tmux.conf.
static void tmux_cmd(const char *cmd) {
    tmux_key(KC_COLN);
    send_string_P(cmd);
    tap_code(KC_ENT);
}

// TREE and COPY put the pane into a real tmux mode rather than just changing
// what the keyboard sends, so the pane has to be taken back out of it before
// anything else happens.
static void tmux_quit_mode(void) {
    if (tmux_mode == _TMUX_TREE || tmux_mode == _TMUX_COPY) {
        tap_code(KC_Q);
    }
}

// Move the layers only, for the keys that have already dealt with tmux
// themselves. tmux_switch_mode below is the one that opens the new mode.
static void tmux_set_mode(uint8_t mode) {
    layer_off(_TMUX_TREE);
    layer_off(_TMUX_WINDOW);
    layer_off(_TMUX_PANE);
    layer_off(_TMUX_COPY);
    if (mode == TMUX_OFF) {
        layer_off(_TMUX);
    } else {
        layer_on(_TMUX);
        layer_on(mode);
    }
    tmux_mode = mode;
    // The thumb modifiers belong to the mode they were pressed in, so a COPY
    // toggle can never survive into PANE.
    tmux_mod = TMOD_NONE;
}

// The four directions differ only in which command each modifier sends, so the
// arrow keys hand their own four variants to this.
static void tmux_pane_arrow(uint16_t arrow, const char *resize, const char *split, const char *move) {
    switch (tmux_mod) {
        case TMOD_RESIZE:
            tmux_cmd(resize);
            break;
        case TMOD_SPLIT:
            tmux_cmd(split);
            break;
        case TMOD_MOVE:
            tmux_cmd(move);
            break;
        default:
            tmux_key(arrow);
            break;
    }
}

// Drop whatever a prompt was holding on to without restoring the mode, either
// because the prompt is over or because TMUX_ON is starting again from scratch.
static void tmux_prompt_cancel(void) {
    if (tmux_prompt_layer != 0) {
        layer_off(tmux_prompt_layer);
        tmux_prompt_layer = 0;
    }
    tmux_prompt = TPROMPT_NONE;
    tmux_prompt_mode = TMUX_OFF;
}

static void tmux_prompt_done(void) {
    uint8_t mode = tmux_prompt_mode;
    tmux_prompt_cancel();
    tmux_set_mode(mode);
}

// The tmux prompt wants the whole keyboard, so this hands it over and waits for
// the user to tap Enter or press Esc.
static void tmux_typing_prompt(void) {
    uint8_t mode = tmux_mode;
    tmux_set_mode(TMUX_OFF);
    tmux_prompt = TPROMPT_TYPING;
    tmux_prompt_mode = mode;
}

// One key goes to tmux and then the mode comes back. PICK needs the digits from
// the OS's own number layer; the tree's y/n comes off the base layer, so layer
// is 0 there.
static void tmux_onekey_prompt(uint8_t layer) {
    uint8_t mode = tmux_mode;
    tmux_set_mode(TMUX_OFF);
    if (layer != 0) {
        layer_on(layer);
    }
    tmux_prompt = TPROMPT_ARMING;
    tmux_prompt_mode = mode;
    tmux_prompt_layer = layer;
}

static uint8_t tmux_num_layer(void) {
    return get_highest_layer(default_layer_state) == _LINUX ? _NUM_LINUX : _NUM_MAC;
}

// I and K are the same key under NEW: name the session, create it, switch to it.
static void tmux_new_session(void) {
    tmux_cmd(PSTR("command-prompt -p \"new session:\" \"new-session -d -s '%%'" TMUX_CWD " \\; switch-client -t '%%'\""));
    tmux_typing_prompt();
}

static void tmux_switch_mode(uint8_t mode) {
    tmux_quit_mode();
    tmux_set_mode(mode);
    if (mode == _TMUX_TREE) {
        // -O activity puts the most recently used first; the preview is on
        // unless -N is given.
        tmux_cmd(PSTR("choose-tree -Zw -O activity"));
    } else if (mode == _TMUX_COPY) {
        tmux_key(KC_LBRC);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // PANE's and WINDOW's thumb modifiers are held, so they are the only keys
    // that have anything to do on the release.
    switch (keycode) {
        case TP_RSZE:
            tmux_mod = record->event.pressed ? TMOD_RESIZE : TMOD_NONE;
            return false;
        case TP_SPLT:
            tmux_mod = record->event.pressed ? TMOD_SPLIT : TMOD_NONE;
            return false;
        case TP_MOVE:
        case TW_MOVE:
            tmux_mod = record->event.pressed ? TMOD_MOVE : TMOD_NONE;
            return false;
        case TW_NEW:
            tmux_mod = record->event.pressed ? TMOD_NEW : TMOD_NONE;
            return false;
    }

    // Prompts end on a release, so that whichever key ended them has already
    // reached tmux by the time the mode comes back.
    if (!record->event.pressed && tmux_prompt != TPROMPT_NONE) {
        switch (tmux_prompt) {
            case TPROMPT_ARMING:
                tmux_prompt = TPROMPT_ONEKEY;
                break;
            case TPROMPT_ONEKEY:
                tmux_prompt_done();
                break;
            case TPROMPT_TYPING:
                // Only a tap of Enter counts, not the hold that reaches the
                // number layer. Anything else the prompt might end with leaves
                // the keyboard where it is, to be re-entered with nav + T.
                if (keycode == KC_ESC || ((keycode == M_ENT || keycode == L_ENT) && record->tap.count > 0)) {
                    tmux_prompt_done();
                }
                break;
        }
        return true;
    }

    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case OS_SWAP:
            if (get_highest_layer(default_layer_state) == _LINUX) {
                set_single_persistent_default_layer(_MAC);
            } else {
                set_single_persistent_default_layer(_LINUX);
            }
            return false;

        // Tapping the current mode's own key quits TREE or COPY and falls back
        // to PANE; in PANE and WINDOW there is nothing open to quit.
        case TMUX_ON:
            tmux_prompt_cancel();
            tmux_set_mode(_TMUX_PANE);
            return false;
        case TM_TREE:
            tmux_switch_mode(tmux_mode == _TMUX_TREE ? _TMUX_PANE : _TMUX_TREE);
            return false;
        case TM_WIN:
            if (tmux_mode != _TMUX_WINDOW) {
                tmux_switch_mode(_TMUX_WINDOW);
            }
            return false;
        case TM_PANE:
            if (tmux_mode != _TMUX_PANE) {
                tmux_switch_mode(_TMUX_PANE);
            }
            return false;
        case TM_COPY:
            tmux_switch_mode(tmux_mode == _TMUX_COPY ? _TMUX_PANE : _TMUX_COPY);
            return false;
        case TM_EXIT:
            // Leaving a tree or copy mode open would send the next thing typed
            // into it instead of the shell.
            tmux_quit_mode();
            tmux_set_mode(TMUX_OFF);
            return false;
        case TM_DTCH:
            tmux_key(KC_D);
            tmux_set_mode(TMUX_OFF);
            return false;

        // swap-pane takes the neighbour as -s rather than -t so that focus ends
        // up on the pane that moved, which is what makes repeated presses push
        // the same pane along.
        case TP_UP:
            tmux_pane_arrow(KC_UP, PSTR("resize-pane -U 5"), PSTR("split-window -vb" TMUX_CWD), PSTR("swap-pane -s '{up-of}'"));
            return false;
        case TP_DOWN:
            tmux_pane_arrow(KC_DOWN, PSTR("resize-pane -D 5"), PSTR("split-window -v" TMUX_CWD), PSTR("swap-pane -s '{down-of}'"));
            return false;
        case TP_LEFT:
            tmux_pane_arrow(KC_LEFT, PSTR("resize-pane -L 5"), PSTR("split-window -hb" TMUX_CWD), PSTR("swap-pane -s '{left-of}'"));
            return false;
        case TP_RGHT:
            tmux_pane_arrow(KC_RGHT, PSTR("resize-pane -R 5"), PSTR("split-window -h" TMUX_CWD), PSTR("swap-pane -s '{right-of}'"));
            return false;
        case TP_LAST:
            tmux_key(KC_SCLN);
            return false;
        case TP_ZOOM:
            tmux_key(KC_Z);
            return false;
        case TP_LYT:
            tmux_key(KC_SPC);
            return false;
        case TP_BRK:
            tmux_key(KC_EXLM);
            return false;
        case TP_JOIN:
            // The tree carries its own template, so it is opened here rather
            // than by switching into TREE mode the usual way.
            tmux_cmd(PSTR("choose-tree -Zw -O activity \"join-pane -h -s '%%'\""));
            tmux_set_mode(_TMUX_TREE);
            return false;
        case TP_PICK:
            tmux_cmd(PSTR("display-panes -d 0"));
            tmux_onekey_prompt(tmux_num_layer());
            return false;

        // MOVE shifts windows, not sessions, so it leaves these two alone.
        case TW_UP:
            if (tmux_mod == TMOD_NEW) {
                tmux_new_session();
            } else if (tmux_mod == TMOD_NONE) {
                tmux_key(KC_LPRN);
            }
            return false;
        case TW_DOWN:
            if (tmux_mod == TMOD_NEW) {
                tmux_new_session();
            } else if (tmux_mod == TMOD_NONE) {
                tmux_key(KC_RPRN);
            }
            return false;
        case TW_LEFT:
            switch (tmux_mod) {
                case TMOD_NEW:
                    tmux_cmd(PSTR("new-window -b" TMUX_CWD));
                    break;
                case TMOD_MOVE:
                    // -d is what makes the client follow the window it moved.
                    tmux_cmd(PSTR("swap-window -d -t -1"));
                    break;
                default:
                    tmux_key(KC_P);
                    break;
            }
            return false;
        case TW_RGHT:
            switch (tmux_mod) {
                case TMOD_NEW:
                    tmux_cmd(PSTR("new-window -a" TMUX_CWD));
                    break;
                case TMOD_MOVE:
                    tmux_cmd(PSTR("swap-window -d -t +1"));
                    break;
                default:
                    tmux_key(KC_N);
                    break;
            }
            return false;
        case TW_LAST:
            tmux_key(KC_L);
            return false;
        case TW_RNS:
            // Renaming a session is only reachable under NEW, next to the key
            // that creates one.
            if (tmux_mod == TMOD_NEW) {
                tmux_key(KC_DLR);
                tmux_typing_prompt();
            }
            return false;
        case TW_SEND:
            if (tmux_mod == TMOD_NEW) {
                tmux_key(KC_COMM);
                tmux_typing_prompt();
                return false;
            }
            // %% is whatever the user picks, so the window being moved has to
            // be marked before the tree opens. -M first because -m on a pane
            // that is already marked clears the mark instead of setting it.
            tmux_cmd(PSTR("select-pane -M ; select-pane -m"));
            tmux_cmd(PSTR("choose-tree -Zs -O activity \"move-window -s '{marked}' -t '%%:' \\; switch-client -t '%%' \\; select-pane -M\""));
            tmux_set_mode(_TMUX_TREE);
            return false;
        case TW_PICK:
            // command-prompt -1 rather than prefix and a digit, so that a stray
            // non-digit cannot fire another prefix binding.
            tmux_cmd(PSTR("command-prompt -1 -p window \"select-window -t ':%%'\""));
            tmux_onekey_prompt(tmux_num_layer());
            return false;

        case TT_SEL:
            // The tree's own template decides what choosing an item means, so
            // there is nothing to know here beyond "the tree is now closed".
            tap_code(KC_ENT);
            tmux_set_mode(_TMUX_PANE);
            return false;
        case TT_CLSE:
            // tmux asks to confirm, and y or n comes straight off the base
            // layer. Closing lives only here; panes go with Ctrl-D.
            tap_code(KC_X);
            tmux_onekey_prompt(0);
            return false;
        case TT_FLTR:
            tap_code(KC_F);
            tmux_typing_prompt();
            return false;

        // WORD leaves up and down as they were; only LINE changes all four.
        case TC_UP:
            tap_code(tmux_mod == TMOD_LINE ? KC_PGUP : KC_UP);
            return false;
        case TC_DOWN:
            tap_code(tmux_mod == TMOD_LINE ? KC_PGDN : KC_DOWN);
            return false;
        case TC_LEFT:
            switch (tmux_mod) {
                case TMOD_WORD:
                    tap_code(KC_B);
                    break;
                case TMOD_LINE:
                    tap_code(KC_0);
                    break;
                default:
                    tap_code(KC_LEFT);
                    break;
            }
            return false;
        case TC_RGHT:
            switch (tmux_mod) {
                case TMOD_WORD:
                    tap_code(KC_W);
                    break;
                case TMOD_LINE:
                    tap_code16(KC_DLR);
                    break;
                default:
                    tap_code(KC_RGHT);
                    break;
            }
            return false;
        case TC_WORD:
            tmux_mod = (tmux_mod == TMOD_WORD) ? TMOD_NONE : TMOD_WORD;
            return false;
        case TC_LINE:
            tmux_mod = (tmux_mod == TMOD_LINE) ? TMOD_NONE : TMOD_LINE;
            return false;
        case TC_SRCH:
            tap_code16(KC_QUES);
            tmux_typing_prompt();
            return false;
        case TC_COPY:
            // Enter is copy-pipe-and-cancel, so copy mode is already gone.
            tap_code(KC_ENT);
            tmux_set_mode(_TMUX_PANE);
            return false;
        case TC_PSTE:
            tap_code(KC_Q);
            tmux_key(KC_RBRC);
            tmux_set_mode(TMUX_OFF);
            return false;
    }

    return true;
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_write_ln_P(PSTR("OS"), false);
        if (get_highest_layer(default_layer_state) == _LINUX) {
            oled_write_ln_P(PSTR("LINUX"), false);
        } else {
            oled_write_ln_P(PSTR("MAC"), false);
        }
        // The tmux layers are toggled, so without this there is no way to tell
        // which mode - or whether tmux mode at all - is on.
        switch (tmux_mode) {
            case _TMUX_TREE:
                oled_write_P(PSTR("TREE"), false);
                break;
            case _TMUX_WINDOW:
                oled_write_P(PSTR("WINDOW"), false);
                break;
            case _TMUX_PANE:
                oled_write_P(PSTR("PANE"), false);
                break;
            case _TMUX_COPY:
                oled_write_P(PSTR("COPY"), false);
                break;
        }
        switch (tmux_mod) {
            case TMOD_RESIZE:
                oled_write_P(PSTR(" RESIZE"), false);
                break;
            case TMOD_SPLIT:
                oled_write_P(PSTR(" SPLIT"), false);
                break;
            case TMOD_MOVE:
                oled_write_P(PSTR(" MOVE"), false);
                break;
            case TMOD_NEW:
                oled_write_P(PSTR(" NEW"), false);
                break;
            case TMOD_WORD:
                oled_write_P(PSTR(" WORD"), false);
                break;
            case TMOD_LINE:
                oled_write_P(PSTR(" LINE"), false);
                break;
        }
        // A prompt has put the mode away, so this is the whole line while one
        // is up.
        switch (tmux_prompt) {
            case TPROMPT_TYPING:
                oled_write_P(PSTR("PROMPT"), false);
                break;
            case TPROMPT_ARMING:
            case TPROMPT_ONEKEY:
                oled_write_P(PSTR("PICK"), false);
                break;
        }
        // Pads out the rest of the line, which is also what clears it when tmux
        // mode is off.
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(is_caps_word_on() ? PSTR("CAPS") : PSTR(""), false);
    }
    return false;
}
#endif
