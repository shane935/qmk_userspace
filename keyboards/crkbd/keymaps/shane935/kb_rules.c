#include "kb_rules.h"

#include "modifiers.h"
#include "quantum_keycodes.h"

uint8_t kb_os_next(uint8_t source) {
    return source == KB_OS_FORCE_LINUX ? KB_OS_FOLLOW : source + 1;
}

uint8_t kb_os_wanted(uint8_t source, uint8_t reported_os) {
    switch (source) {
        case KB_OS_FORCE_MAC:
            return KB_OS_MAC;
        case KB_OS_FORCE_LINUX:
            return KB_OS_LINUX;
        default:
            // Anything other than a definite answer leaves the layer alone,
            // which also covers a host that has reported nothing at all.
            return (reported_os == KB_OS_LINUX || reported_os == KB_OS_MAC) ? reported_os : KB_OS_UNKNOWN;
    }
}

bool kb_host_alive(uint32_t last_ms, uint32_t now_ms) {
    // Unsigned subtraction, so a timer that has wrapped past the last report
    // still gives the real elapsed time rather than an enormous one.
    return last_ms != 0 && (uint32_t)(now_ms - last_ms) < KB_HOST_TIMEOUT_MS;
}

bool kb_adopt_pane(const kb_context_t *ctx, bool claims_mode) {
    if (!claims_mode || !(ctx->tmux_bits & KB_TMUX_PRESENT)) {
        return false;
    }
    if (ctx->tmux_bits & (KB_TMUX_COPY_MODE | KB_TMUX_OTHER_MODE)) {
        return false;
    }
    // An intent in flight is an explanation, so the world has not moved.
    return ctx->intent_status == KB_INTENT_NONE;
}

bool kb_intent_settled(const kb_context_t *ctx, uint8_t nonce) {
    return ctx->intent_nonce == nonce && (ctx->intent_status == KB_INTENT_DONE || ctx->intent_status == KB_INTENT_FAILED);
}

// The three modifiers the key table uses, as the bits a keycode carries them in.
#define KB_ROW_QK_MODS (QK_LCTL | QK_LSFT | QK_LALT)

bool kb_is_tmux_row(uint16_t keycode) {
    uint8_t basic = keycode & 0xFF;
    if (basic < KC_F13 || basic > KC_F24) {
        return false;
    }
    // Nothing set outside the basic byte and those three modifiers, so a
    // tap-hold or layer keycode that happens to end in an F-key is not one.
    return (keycode & ~(uint16_t)(0x00FF | KB_ROW_QK_MODS)) == 0;
}

uint8_t kb_tmux_row_key(uint16_t keycode) {
    return keycode & 0xFF;
}

uint8_t kb_tmux_row_mods(uint16_t keycode) {
    // QK_LCTL/LSFT/LALT sit exactly one byte above MOD_LCTL/LSFT/LALT, so the
    // shift is the whole conversion. kb_rules_test pins that rather than
    // trusting it.
    return (keycode >> 8) & (MOD_LCTL | MOD_LSFT | MOD_LALT);
}
