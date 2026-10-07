#include QMK_KEYBOARD_H
#ifdef RAW_ENABLE
#    include "raw_hid.h"
#endif

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
    _TMUX_APP,
    _TMUX_COPY,
};

enum custom_keycodes {
    OS_SWAP = SAFE_RANGE,
    // Mode switching, shared by every tmux layer.
    TMUX_ON,
    TM_TREE,
    TM_WIN,
    TM_PANE,
    TM_APP,
    TM_COPY,
    TM_EXIT,
    TM_DTCH,
    // Not a mode key, but it ends in one: it opens Claude's transcript viewer
    // and lands in APP mode with it. Only PANE has it.
    TM_TRSC,
    // PANE mode.
    TP_UP,
    TP_DOWN,
    TP_LEFT,
    TP_RGHT,
    TP_LAST,
    TP_ZOOM,
    TP_LYT,
    TP_RSZE,
    TP_SPLT,
    TP_MOVE,
    // WINDOW mode.
    TW_UP,
    TW_DOWN,
    TW_LEFT,
    TW_RGHT,
    TW_LAST,
    TW_MOVE,
    // TREE mode.
    TT_SEL,
    TT_KILL,
    // COPY mode.
    TC_UP,
    TC_DOWN,
    TC_LEFT,
    TC_RGHT,
    TC_COPY,
    TC_PSTE,
    TC_WORD,
    TC_LINE,
    // APP mode. It drives the program in the pane rather than tmux, so every
    // one of these sends keys to the program with no prefix in front of them.
    TA_UP,
    TA_DOWN,
    TA_LEFT,
    TA_RGHT,
    TA_NEXT,
    TA_PREV,
    TA_SRCH,
    TA_TRSC,
    TA_HALF,
    TA_FULL,
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
          TM_TREE,       TM_WIN,      TM_PANE,       TM_APP,      TM_COPY,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
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
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     XXXXXXX,   A(KC_MINS),        KC_UP,   A(KC_PLUS),      TT_KILL,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      _______,                                     XXXXXXX,      KC_LEFT,      KC_DOWN,      KC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      XXXXXXX,         XXXXXXX,       TT_SEL,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_TMUX_WINDOW] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     TW_LAST,      XXXXXXX,        TW_UP,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      _______,                                     XXXXXXX,      TW_LEFT,      TW_DOWN,      TW_RGHT,      TM_DTCH,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      XXXXXXX,      TW_MOVE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    [_TMUX_PANE] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     TP_LAST,      TP_ZOOM,        TP_UP,      TM_TRSC,       TP_LYT,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      _______,                                     XXXXXXX,      TP_LEFT,      TP_DOWN,      TP_RGHT,      TM_DTCH,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    TP_RSZE,      TP_SPLT,      TP_MOVE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    // App mode drives the program in the focused pane instead of tmux, so
    // nothing here goes through the prefix. What each key sends depends on which
    // program tmux_app says is there and, for Claude, on whether its transcript
    // viewer is open; the tables in process_record_user are the whole story.
    [_TMUX_APP] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                     TA_NEXT,      XXXXXXX,        TA_UP,      TA_TRSC,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      _______,                                     TA_PREV,      TA_LEFT,      TA_DOWN,      TA_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      TA_SRCH,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    TA_HALF,      TA_FULL,      XXXXXXX,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  ),

    // Copy mode drives tmux's copy-mode-vi key table, so the unmodified keys go
    // straight through as the vi keys they are.
    [_TMUX_COPY] = LAYOUT_split_3x5_3(
  //,---------------------------------------------------------------------.                              ,---------------------------------------------------------------------.
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      _______,                                        KC_N,       KC_SPC,        TC_UP,      S(KC_V),       KC_ESC,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          _______,      _______,      _______,      _______,      _______,                                     S(KC_N),      TC_LEFT,      TC_DOWN,      TC_RGHT,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------|                              |-------------+-------------+-------------+-------------+-------------|
          XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                     XXXXXXX,      XXXXXXX,      TC_COPY,      TC_PSTE,      XXXXXXX,
  //|-------------+-------------+-------------+-------------+-------------+-------------|  |-------------+-------------+-------------+-------------+-------------+-------------|
                                                    XXXXXXX,      TC_WORD,      TC_LINE,         XXXXXXX,      XXXXXXX,       XXXXXXX
                                            //`-----------------------------------------'  `-----------------------------------------'
  )
};
// clang-format on

// Which mode layer sits on top of _TMUX, or TMUX_OFF when tmux mode is off.
// Layer 0 is _MAC and so can never be a mode, which leaves it free to mean off.
#define TMUX_OFF 0
static uint8_t tmux_mode = TMUX_OFF;

// The left thumb modifier in effect. PANE, WINDOW and APP hold theirs, COPY
// toggles its own, and only ever one at a time. PANE and WINDOW both call their
// inner thumb MOVE and neither can be on while the other is, so they share a
// value. These are the values STATE byte 9 carries, so 4 stays empty rather
// than being reused: the protocol reserved it when WINDOW's NEW thumb went.
enum tmux_modifier {
    TMOD_NONE,
    TMOD_RESIZE,
    TMOD_SPLIT,
    TMOD_MOVE,
    TMOD_WORD = 5,
    TMOD_LINE,
    TMOD_HALF,
    TMOD_FULL,
};
static uint8_t tmux_mod = TMOD_NONE;

// Which program APP mode is driving. It outlives a mode switch, so coming back
// to APP lands on whatever was last selected, and the mode key is what cycles
// it.
enum tmux_apps {
    TAPP_CLAUDE,
    TAPP_HUNK,
};
static uint8_t tmux_app = TAPP_CLAUDE;

// Whether Claude's transcript viewer is open. Claude draws on the alternate
// screen, so the keyboard cannot see what state it is in and has to remember.
// TA_TRSC is the only way it becomes true, and it becomes true by sending the
// Ctrl-O that opens the viewer, so the two cannot disagree unless Claude's own
// Esc closes it. A stale true is the dangerous direction -- it is what would
// send Ctrl-D to a live prompt -- so the mode key can only ever set it false.
static bool tmux_transcript = false;

// Killing raises tmux's own "(y/n)" prompt, and the tree layer has no y on it
// to answer with. So a kill is two presses of the same key: one to ask, one to
// answer. True between those two presses.
static bool tmux_kill_pending = false;

// The protocol in docs/00-protocol.md. Every report in both directions is
// exactly 32 bytes, zero padded -- a protocol size, not QMK's: RAW_EPSIZE is
// also 32, but it lives in a tmk_core header keymaps do not get and is 8 on a
// vusb board.
#define KB_REPORT_SIZE 32
#define KB_MAGIC 0xA5
#define KB_VERSION 2
#define KB_HELLO 0x01
#define KB_STATE 0x02
#define KB_CONTEXT 0x81
#define KB_SEQ_NONE 0xFF
// The host sends a CONTEXT every 500 ms whether anything changed or not, so
// this is three missed reports.
#define KB_HOST_TIMEOUT_MS 1500

// STATE byte 8. Not the layer numbers: the protocol numbers the modes itself,
// and knows nothing of the keyboard's layer stack.
enum kb_mode_byte { KB_MODE_OFF, KB_MODE_TMUX, KB_MODE_TREE, KB_MODE_WINDOW, KB_MODE_PANE, KB_MODE_WITHIN };
// STATE byte 10.
enum kb_os_byte { KB_OS_UNKNOWN, KB_OS_LINUX, KB_OS_MAC };
#define KB_OS_OVERRIDE 0x80
// STATE byte 12.
enum kb_event_byte { KB_EVENT_NONE, KB_EVENT_CLAUDE_TRSC, KB_EVENT_TMUX_KEY, KB_EVENT_COPY_KEY };
// CONTEXT byte 7.
#define KB_TMUX_PRESENT 0x01
#define KB_TMUX_COPY_MODE 0x04
#define KB_TMUX_OTHER_MODE 0x08
// CONTEXT byte 12.
enum kb_intent_status { KB_INTENT_NONE, KB_INTENT_PENDING, KB_INTENT_DONE, KB_INTENT_FAILED };

// Everything the host owns. Reports only: nothing in here is ever a command,
// and the keyboard's own desired state above is never overwritten by it except
// through the rules in raw_hid_receive.
static struct {
    uint32_t last_ctx_ms;
    uint8_t  os, program, tmux_bits, transcript, phase, perm;
    uint8_t  intent_nonce, intent_status;
    uint8_t  window, pane;
    char     label[17];
    uint8_t  host_seq, my_seq, nonce;
} ctx = {.host_seq = KB_SEQ_NONE};

// What the keyboard just did, for the host to schedule its observation around.
// It describes one keypress, so send_state clears it on the way out.
static uint8_t kb_event, kb_event_arg;

// True once OS_SWAP has been pressed. The host is otherwise the authority on
// which OS it is, and this is the one thing that outranks it.
static bool kb_os_override = false;

// Evaluated at the keypress and never cached: a mode entered while the host was
// alive can be left after it has died. last_ctx_ms is zero until the first
// CONTEXT, which stops the first 1.5 s after boot from reading as alive.
static bool host_alive(void) {
    return ctx.last_ctx_ms != 0 && timer_elapsed32(ctx.last_ctx_ms) < KB_HOST_TIMEOUT_MS;
}

static uint8_t kb_mode_of(uint8_t mode) {
    switch (mode) {
        case _TMUX_TREE:
            return KB_MODE_TREE;
        case _TMUX_WINDOW:
            return KB_MODE_WINDOW;
        case _TMUX_PANE:
            return KB_MODE_PANE;
        // The protocol has one WITHIN where the keymap still has APP and COPY.
        // They become one layer in a later change; until then both report as
        // the mode they are turning into.
        case _TMUX_APP:
        case _TMUX_COPY:
            return KB_MODE_WITHIN;
        default:
            return KB_MODE_OFF;
    }
}

// TMOD_* already carry the protocol's values, with 4 the hole where NEW was.
// HALF and FULL have no value because the protocol does not have them: they are
// what WORD and LINE become when APP folds into WITHIN, so they report as that.
static uint8_t kb_mod_of(uint8_t mod) {
    switch (mod) {
        case TMOD_HALF:
            return TMOD_WORD;
        case TMOD_FULL:
            return TMOD_LINE;
        default:
            return mod;
    }
}

#ifdef RAW_ENABLE
// Sent on every change to a field the keyboard owns, on every intent, on
// connect, and whenever a CONTEXT turns out to be answering an older STATE.
static void send_state_msg(uint8_t type, uint8_t intent, uint8_t arg) {
    uint8_t report[KB_REPORT_SIZE] = {0};
    report[0]                  = KB_MAGIC;
    report[1]                  = KB_VERSION;
    report[2]                  = ++ctx.my_seq;
    report[3]                  = ctx.host_seq;
    report[4]                  = type;
    report[5]                  = intent;
    report[6]                  = arg;
    // The nonce is what makes an intent one-shot: the host actuates once per
    // nonce, so a resend of the same intent is a resend and not a second go.
    report[7]  = intent ? ++ctx.nonce : ctx.nonce;
    report[8]  = kb_mode_of(tmux_mode);
    report[9]  = kb_mod_of(tmux_mod);
    report[10] = (get_highest_layer(default_layer_state) == _LINUX ? KB_OS_LINUX : KB_OS_MAC) | (kb_os_override ? KB_OS_OVERRIDE : 0);
    report[11] = host_alive() ? 0x01 : 0x00;
    report[12] = kb_event;
    report[13] = kb_event_arg;
    raw_hid_send(report, sizeof(report));
    kb_event     = KB_EVENT_NONE;
    kb_event_arg = 0;
}

static void send_state(uint8_t intent, uint8_t arg) {
    send_state_msg(KB_STATE, intent, arg);
}
#else
#    define send_state(intent, arg) ((void)0)
#endif

// Every tmux action is one row of the key table in docs/00-protocol.md: a
// single root-table key, which tmux runs the instant it arrives. The commands
// themselves -- and so the directory a new pane opens in, and every other flag
// -- live in the generated tmux.conf, not here. mods names the row, so S-F19 in
// the table is tmux_fkey(KC_F19, MOD_LSFT) here.
static void tmux_fkey(uint8_t fkey, uint8_t mods) {
    // register_mods ignores an empty mask and sends the report itself, so the
    // modifier is down in the same report as the function key rather than beside
    // it, which is the whole point of sending it this way.
    register_mods(mods);
    tap_code(fkey);
    unregister_mods(mods);
    // The host never actuates this -- tmux already has. It says which row went
    // out so the host knows what to watch for, and in the same encoding the
    // table uses: the row number, plus a bit per modifier.
    kb_event     = KB_EVENT_TMUX_KEY;
    kb_event_arg = (fkey - KC_F13 + 1) | ((mods & MOD_LSFT) ? 0x10 : 0) | ((mods & MOD_LCTL) ? 0x20 : 0) | ((mods & MOD_LALT) ? 0x40 : 0);
    send_state(0, 0);
}

// TREE and COPY put the pane into a real tmux mode rather than just changing
// what the keyboard sends, so the pane has to be taken back out of it before
// anything else happens.
static void tmux_quit_mode(void) {
    if (tmux_mode == _TMUX_TREE || tmux_mode == _TMUX_COPY) {
        tmux_fkey(KC_F20, 0);
    }
}

// Move the layers only, for the keys that have already dealt with tmux
// themselves. tmux_switch_mode below is the one that opens the new mode.
static void tmux_set_mode(uint8_t mode) {
    layer_off(_TMUX_TREE);
    layer_off(_TMUX_WINDOW);
    layer_off(_TMUX_PANE);
    layer_off(_TMUX_APP);
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
    send_state(0, 0);
}

// The host mirrors the held modifier into its status line, so it has to hear
// about the release as well as the press.
static void tmux_set_mod(uint8_t mod) {
    tmux_mod = mod;
    send_state(0, 0);
}

// The four directions differ only in which row they send, and the modifiers
// only in which mod rides on it: Shift resizes, Ctrl splits, Alt swaps. That is
// the whole of the table's PANE block.
static void tmux_pane_arrow(uint8_t fkey) {
    switch (tmux_mod) {
        case TMOD_RESIZE:
            tmux_fkey(fkey, MOD_LSFT);
            break;
        case TMOD_SPLIT:
            tmux_fkey(fkey, MOD_LCTL);
            break;
        case TMOD_MOVE:
            tmux_fkey(fkey, MOD_LALT);
            break;
        default:
            tmux_fkey(fkey, 0);
            break;
    }
}

// One APP key's row of the size table: what it sends with nothing held, with
// HALF held and with FULL held. KC_NO means it sends nothing in that state.
typedef struct {
    uint16_t bare;
    uint16_t half;
    uint16_t full;
} tmux_app_row_t;

// The arrows take one row per state the layer can be read in, because the same
// jump is a different key to Claude's prompt, Claude's transcript viewer and
// hunk. Everything context-dependent about the layer goes through here.
static void tmux_app_arrow(tmux_app_row_t viewer, tmux_app_row_t prompt, tmux_app_row_t hunk) {
    tmux_app_row_t row     = (tmux_app == TAPP_HUNK) ? hunk : (tmux_transcript ? viewer : prompt);
    uint16_t       keycode = (tmux_mod == TMOD_HALF) ? row.half : (tmux_mod == TMOD_FULL) ? row.full : row.bare;
    if (keycode != KC_NO) {
        tap_code16(keycode);
    }
}

// n, N and / are only keys inside a viewer. In Claude's prompt with the
// transcript closed they would be typed into the message instead, so there they
// send nothing at all.
static void tmux_app_search(uint16_t keycode) {
    if (tmux_app == TAPP_HUNK || tmux_transcript) {
        tap_code16(keycode);
    }
}

// Ctrl-O is Claude's own transcript toggle and the only thing this layer ever
// sends it: Esc would interrupt whatever turn is running.
static void tmux_app_toggle_transcript(void) {
    tap_code16(C(KC_O));
    tmux_transcript = !tmux_transcript;
}

static void tmux_switch_mode(uint8_t mode) {
    tmux_quit_mode();
    tmux_set_mode(mode);
    if (mode == _TMUX_TREE) {
        tmux_fkey(KC_F21, 0);
    } else if (mode == _TMUX_COPY) {
        tmux_fkey(KC_F19, 0);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // The kill prompt is tmux's, not the keyboard's: while it is up it swallows
    // the next key whatever that key was meant for. So every key except the one
    // that answers it says no first and then does its own job, and the prompt
    // can never outlive the press that raised it.
    if (record->event.pressed && tmux_kill_pending && keycode != TT_KILL) {
        tap_code(KC_N);
        tmux_kill_pending = false;
    }

    // PANE's, WINDOW's and APP's thumb modifiers are held, so they are the only
    // keys that have anything to do on the release.
    switch (keycode) {
        case TP_RSZE:
            tmux_set_mod(record->event.pressed ? TMOD_RESIZE : TMOD_NONE);
            return false;
        case TP_SPLT:
            tmux_set_mod(record->event.pressed ? TMOD_SPLIT : TMOD_NONE);
            return false;
        case TP_MOVE:
        case TW_MOVE:
            tmux_set_mod(record->event.pressed ? TMOD_MOVE : TMOD_NONE);
            return false;
        case TA_HALF:
            tmux_set_mod(record->event.pressed ? TMOD_HALF : TMOD_NONE);
            return false;
        case TA_FULL:
            tmux_set_mod(record->event.pressed ? TMOD_FULL : TMOD_NONE);
            return false;
    }

    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        // Pressing this says the user knows better than the host's report, so
        // from here on the host's os is recorded and shown but not applied.
        case OS_SWAP:
            kb_os_override = true;
            if (get_highest_layer(default_layer_state) == _LINUX) {
                set_single_persistent_default_layer(_MAC);
            } else {
                set_single_persistent_default_layer(_LINUX);
            }
            send_state(0, 0);
            return false;

        // Tapping the current mode's own key quits TREE or COPY and falls back
        // to PANE; in PANE and WINDOW there is nothing open to quit.
        case TMUX_ON:
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
        // Once in APP, the same key flips which program is being driven rather
        // than leaving the mode. It only ever declares -- it sends nothing,
        // because the pane a keystroke would land in is not necessarily the one
        // the state is being moved to. Flipping to Claude also declares its
        // viewer closed, which is both the resync for a viewer Claude's own Esc
        // closed and the only state this key can put the flag into: false is the
        // safe direction to be wrong in, and TA_TRSC is the only way to true.
        case TM_APP:
            if (tmux_mode != _TMUX_APP) {
                tmux_switch_mode(_TMUX_APP);
            } else if (tmux_app == TAPP_CLAUDE) {
                tmux_app = TAPP_HUNK;
            } else {
                tmux_app        = TAPP_CLAUDE;
                tmux_transcript = false;
            }
            return false;
        case TM_COPY:
            // Inside Claude's transcript viewer [ writes the whole conversation
            // into the terminal's own scrollback, which is the only way copy
            // mode gets to see more than the frame Claude is currently drawing.
            if (tmux_mode == _TMUX_APP && tmux_app == TAPP_CLAUDE && tmux_transcript) {
                tap_code(KC_LBRC);
            }
            tmux_switch_mode(tmux_mode == _TMUX_COPY ? _TMUX_PANE : _TMUX_COPY);
            return false;
        case TM_EXIT:
            // Leaving a tree or copy mode open would send the next thing typed
            // into it instead of the shell.
            tmux_quit_mode();
            tmux_set_mode(TMUX_OFF);
            return false;
        case TM_DTCH:
            tmux_fkey(KC_F17, MOD_LCTL);
            tmux_set_mode(TMUX_OFF);
            return false;
        // The way in to Claude. Ctrl-O is a toggle, so the flag toggles with it
        // rather than being forced on: press it in a pane whose viewer is
        // already open and this closes it, with APP still agreeing about which.
        case TM_TRSC:
            tmux_app_toggle_transcript();
            tmux_app = TAPP_CLAUDE;
            tmux_switch_mode(_TMUX_APP);
            return false;

        case TP_UP:
            tmux_pane_arrow(KC_F15);
            return false;
        case TP_DOWN:
            tmux_pane_arrow(KC_F14);
            return false;
        case TP_LEFT:
            tmux_pane_arrow(KC_F13);
            return false;
        case TP_RGHT:
            tmux_pane_arrow(KC_F16);
            return false;
        case TP_LAST:
            tmux_fkey(KC_F17, 0);
            return false;
        case TP_ZOOM:
            tmux_fkey(KC_F18, 0);
            return false;
        case TP_LYT:
            tmux_fkey(KC_F18, MOD_LSFT);
            return false;

        // Up and down are sessions, which MOVE has nothing to say about, so
        // they are dead while it is held.
        case TW_UP:
            if (tmux_mod == TMOD_NONE) {
                tmux_fkey(KC_F19, MOD_LSFT);
            }
            return false;
        case TW_DOWN:
            if (tmux_mod == TMOD_NONE) {
                tmux_fkey(KC_F20, MOD_LSFT);
            }
            return false;
        case TW_LEFT:
            tmux_fkey(KC_F22, tmux_mod == TMOD_MOVE ? MOD_LSFT : 0);
            return false;
        case TW_RGHT:
            tmux_fkey(KC_F23, tmux_mod == TMOD_MOVE ? MOD_LSFT : 0);
            return false;
        case TW_LAST:
            tmux_fkey(KC_F21, MOD_LSFT);
            return false;

        case TT_SEL:
            // The tree's own template decides what choosing an item means, so
            // there is nothing to know here beyond "the tree is now closed".
            tap_code(KC_ENT);
            tmux_set_mode(_TMUX_PANE);
            return false;
        // A bare x, because the tree is reading keys itself rather than through
        // the prefix. It kills whatever is highlighted, session or window, and
        // the tree stays open afterwards.
        case TT_KILL:
            if (tmux_kill_pending) {
                tap_code(KC_Y);
            } else {
                tap_code(KC_X);
            }
            tmux_kill_pending = !tmux_kill_pending;
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
            tmux_set_mod((tmux_mod == TMOD_WORD) ? TMOD_NONE : TMOD_WORD);
            return false;
        case TC_LINE:
            tmux_set_mod((tmux_mod == TMOD_LINE) ? TMOD_NONE : TMOD_LINE);
            return false;
        case TC_COPY:
            // Enter is copy-pipe-and-cancel, so copy mode is already gone.
            tap_code(KC_ENT);
            tmux_set_mode(_TMUX_PANE);
            return false;
        case TC_PSTE:
            tmux_fkey(KC_F20, 0);
            tmux_fkey(KC_F22, MOD_LCTL);
            tmux_set_mode(TMUX_OFF);
            return false;

        // The four arrows, each handing over its three rows in the order the
        // viewer, the prompt and hunk read them. Arrows rather than j/k: both
        // programs accept arrows, and an arrow is harmless in Claude's prompt
        // where a letter would be typed into the message.
        case TA_UP:
            tmux_app_arrow((tmux_app_row_t){KC_UP, C(KC_U), KC_B}, (tmux_app_row_t){KC_UP, KC_PGUP, KC_PGUP}, (tmux_app_row_t){KC_UP, KC_U, KC_B});
            return false;
        case TA_DOWN:
            tmux_app_arrow((tmux_app_row_t){KC_DOWN, C(KC_D), KC_SPC}, (tmux_app_row_t){KC_DOWN, KC_PGDN, KC_PGDN}, (tmux_app_row_t){KC_DOWN, KC_D, KC_SPC});
            return false;
        // Left and right jump between things rather than by distance, so the
        // sizes read as prompt, annotated hunk, file. Claude's prompt has
        // nothing to jump between, so there they are dead whatever is held.
        case TA_LEFT:
            tmux_app_arrow((tmux_app_row_t){KC_LCBR, KC_LCBR, KC_LCBR}, (tmux_app_row_t){KC_NO, KC_NO, KC_NO}, (tmux_app_row_t){KC_LBRC, KC_LCBR, KC_COMM});
            return false;
        case TA_RGHT:
            tmux_app_arrow((tmux_app_row_t){KC_RCBR, KC_RCBR, KC_RCBR}, (tmux_app_row_t){KC_NO, KC_NO, KC_NO}, (tmux_app_row_t){KC_RBRC, KC_RCBR, KC_DOT});
            return false;
        case TA_NEXT:
            tmux_app_search(KC_N);
            return false;
        case TA_PREV:
            tmux_app_search(S(KC_N));
            return false;
        case TA_SRCH:
            tmux_app_search(KC_SLSH);
            return false;
        // The only key that opens or closes the viewer, and so the only one that
        // moves the flag by acting rather than by declaring. hunk has no second
        // screen to open, so it is dead there.
        case TA_TRSC:
            if (tmux_app == TAPP_CLAUDE) {
                tmux_app_toggle_transcript();
            }
            return false;
    }

    return true;
}

#ifdef RAW_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (length < KB_REPORT_SIZE || data[0] != KB_MAGIC || data[1] != KB_VERSION || data[4] != KB_CONTEXT) {
        return;
    }
    ctx.host_seq      = data[2];
    ctx.os            = data[5];
    ctx.program       = data[6];
    ctx.tmux_bits     = data[7];
    ctx.transcript    = data[8];
    ctx.phase         = data[9];
    ctx.perm          = data[10];
    ctx.intent_nonce  = data[11];
    ctx.intent_status = data[12];
    ctx.window        = data[13];
    ctx.pane          = data[14];
    memcpy(ctx.label, &data[15], 16);
    ctx.label[16] = '\0';
    // host_alive reads zero as "no CONTEXT yet", and timer_read32 can
    // legitimately return zero, so that one value is never stored.
    ctx.last_ctx_ms = timer_read32();
    if (ctx.last_ctx_ms == 0) {
        ctx.last_ctx_ms = 1;
    }

    // The host is the only authority on which OS it is running on. OS_SWAP
    // outranks it; nothing else does.
    if (!kb_os_override && (ctx.os == KB_OS_LINUX || ctx.os == KB_OS_MAC)) {
        uint8_t want = (ctx.os == KB_OS_LINUX) ? _LINUX : _MAC;
        if (get_highest_layer(default_layer_state) != want) {
            set_single_persistent_default_layer(want);
        }
    }

    // The safety net, not a normal path. The world can move without the
    // keyboard -- a pane closes, a program exits, someone presses q in copy
    // mode -- and a report that no mode is open while the keyboard believes it
    // is in one, with no intent in flight to explain it, means the keyboard is
    // the one that is wrong. tmux_set_mode sends the corrected state itself.
    if ((ctx.tmux_bits & KB_TMUX_PRESENT) && !(ctx.tmux_bits & (KB_TMUX_COPY_MODE | KB_TMUX_OTHER_MODE)) && ctx.intent_status == KB_INTENT_NONE && (tmux_mode == _TMUX_TREE || tmux_mode == _TMUX_COPY)) {
        tmux_set_mode(_TMUX_PANE);
        return;
    }

    // A CONTEXT whose ack is not the current seq answered an older STATE, so
    // say the current one again. The host acks the latest report it has seen
    // rather than the one it is replying to, so a STATE that crossed a CONTEXT
    // in flight costs one extra round trip and then settles.
    if (data[3] != ctx.my_seq) {
        send_state(0, 0);
    }
}

void keyboard_post_init_user(void) {
    send_state_msg(KB_HELLO, 0, 0);
}
#endif

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
            // APP prints the program rather than the mode, because which one it
            // is decides what every key on the layer sends -- and for Claude,
            // whether the transcript viewer is open decides it too.
            case _TMUX_APP:
                if (tmux_app == TAPP_HUNK) {
                    oled_write_P(PSTR("HUNK"), false);
                } else if (tmux_transcript) {
                    oled_write_P(PSTR("TRSC"), false);
                } else {
                    oled_write_P(PSTR("CLAUDE"), false);
                }
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
            case TMOD_WORD:
                oled_write_P(PSTR(" WORD"), false);
                break;
            case TMOD_LINE:
                oled_write_P(PSTR(" LINE"), false);
                break;
            case TMOD_HALF:
                oled_write_P(PSTR(" HALF"), false);
                break;
            case TMOD_FULL:
                oled_write_P(PSTR(" FULL"), false);
                break;
        }
        // No fresh report from the host, so the keyboard has no idea what is in
        // the pane: WITHIN is plain copy mode and nothing resolves. Worth a mark
        // of its own, because every key still works and only the smart ones are
        // missing.
        if (tmux_mode != TMUX_OFF && !host_alive()) {
            oled_write_P(PSTR(" ~"), false);
        }
        // A kill is waiting on its second press. tmux is showing its own prompt
        // too, but that is down in the status line and easy to miss.
        if (tmux_kill_pending) {
            oled_write_P(PSTR(" KILL?"), false);
        }
        // Pads out the rest of the line, which is also what clears it when tmux
        // mode is off.
        oled_write_ln_P(PSTR(""), false);
        oled_write_ln_P(is_caps_word_on() ? PSTR("CAPS") : PSTR(""), false);
    }
    return false;
}
#endif
