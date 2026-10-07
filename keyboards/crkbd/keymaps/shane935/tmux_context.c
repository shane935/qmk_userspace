#include "tmux_context.h"

#include "quantum_keycodes.h"

// Shorthands for the three shapes an answer takes, so the tables below read as
// tables rather than as struct literals.
#define SENDS(kc) ((within_action_t){.key = (kc)})
#define SENDS2(kc, kc2) ((within_action_t){.key = (kc), .then = (kc2)})
#define NOTHING ((within_action_t){0})
#define INTENT(id) ((within_action_t){.intent = (id)})

within_target_t within_target(const kb_context_t *ctx, bool host_alive) {
    // No fresh report, or no tmux at all: the keyboard knows nothing about the
    // pane, so copy mode is all it can honestly offer.
    if (!host_alive || !(ctx->tmux_bits & KB_TMUX_PRESENT)) {
        return WT_COPY;
    }
    // A pane already in copy mode is copy mode whatever is running underneath
    // it, because copy mode is what has the keyboard.
    if (ctx->tmux_bits & KB_TMUX_COPY_MODE) {
        return WT_COPY;
    }
    if (ctx->program == KB_PROGRAM_CLAUDE) {
        return ctx->transcript == KB_TRANSCRIPT_OPEN ? WT_TRANSCRIPT : WT_CLAUDE_SAFE;
    }
    if (ctx->program == KB_PROGRAM_HUNK) {
        return WT_HUNK;
    }
    return WT_COPY;
}

// tmux's copy-mode-vi keys, which is what the COPY layer has always sent. WORD
// leaves up and down alone; only LINE changes all four.
static within_action_t resolve_copy(within_key_t key, within_mod_t mod) {
    switch (key) {
        case WK_UP:
            return SENDS(mod == WM_LINE ? KC_PGUP : KC_UP);
        case WK_DOWN:
            return SENDS(mod == WM_LINE ? KC_PGDN : KC_DOWN);
        case WK_LEFT:
            return SENDS(mod == WM_WORD ? KC_B : mod == WM_LINE ? KC_0 : KC_LEFT);
        case WK_RIGHT:
            return SENDS(mod == WM_WORD ? KC_W : mod == WM_LINE ? KC_DLR : KC_RGHT);
        case WK_SEARCH:
            return SENDS(KC_SLSH);
        case WK_NEXT:
            return SENDS(KC_N);
        case WK_PREV:
            return SENDS(S(KC_N));
        // Escape, which copy-mode-vi reads as clear-selection: it drops a
        // selection you did not mean and leaves the cursor where it is. Leaving
        // copy mode is D, which goes to PANE as well, so this key is better
        // spent on the thing nothing else can do.
        case WK_LEAVE:
            return SENDS(KC_ESC);
        // The two selections. Only copy mode has any, and a bare Space or V
        // anywhere else would go into the program -- which is why they resolve
        // here with everything else rather than sitting on the layer as plain
        // keycodes.
        case WK_MARK:
            return SENDS(KC_SPC);
        case WK_SELECT:
            return SENDS(S(KC_V));
        case WK_COPY:
            // copy-pipe-and-cancel, so copy mode closes itself.
            return SENDS(KC_ENT);
        case WK_PASTE:
            return SENDS2(KC_F20, C(KC_F22));
        case WK_ENTER:
            return SENDS(KC_F19);
        default:
            return NOTHING;
    }
}

// Claude's transcript viewer, observed open. The unit for left and right is the
// prompt, which is the same jump whatever thumb is held.
static within_action_t resolve_transcript(within_key_t key, within_mod_t mod) {
    switch (key) {
        case WK_UP:
            return SENDS(mod == WM_WORD ? C(KC_U) : mod == WM_LINE ? KC_B : KC_UP);
        case WK_DOWN:
            return SENDS(mod == WM_WORD ? C(KC_D) : mod == WM_LINE ? KC_SPC : KC_DOWN);
        case WK_LEFT:
            return SENDS(KC_LCBR);
        case WK_RIGHT:
            return SENDS(KC_RCBR);
        case WK_SEARCH:
            return SENDS(KC_SLSH);
        case WK_NEXT:
            return SENDS(KC_N);
        case WK_PREV:
            return SENDS(S(KC_N));
        // Ctrl-O is the viewer's own toggle. Escape would interrupt the turn.
        case WK_LEAVE:
            return SENDS(C(KC_O));
        // A tmux key rather than the chord itself. Claude's own C-x C-b ends in
        // tmux's default prefix, so typing it directly would have tmux eat the
        // C-b and take the next key as a prefix command; C-F23 is bound to
        // send-keys, which writes into the pane past tmux's key tables.
        case WK_BKGD:
            return SENDS(C(KC_F23));
        // Entering WITHIN again, from a viewer the host has already confirmed
        // open, asks for the whole conversation: [ writes it into the terminal's
        // own scrollback so copy mode can see all of it rather than the one frame
        // Claude is drawing. An intent because only the host can tell when the
        // writing has stopped. So F escalates -- once for the viewer, again for
        // everything in it -- and this needs no key of its own.
        case WK_ENTER:
            return INTENT(KB_INTENT_WITHIN_DEEP);
        default:
            return NOTHING;
    }
}

// A Claude pane whose viewer is not confirmed open, so every keystroke has to
// survive landing in a live prompt. Arrows and page keys do; letters would be
// typed into the message, Escape would interrupt the turn, and Ctrl-D would exit
// Claude Code. So those send nothing at all.
static within_action_t resolve_claude_safe(within_key_t key, within_mod_t mod) {
    switch (key) {
        case WK_UP:
            return SENDS(mod == WM_NONE ? KC_UP : KC_PGUP);
        case WK_DOWN:
            return SENDS(mod == WM_NONE ? KC_DOWN : KC_PGDN);
        case WK_BKGD:
            return SENDS(C(KC_F23));
        // Opening the viewer is the whole point of entering WITHIN on a Claude
        // pane. An intent rather than the Ctrl-O itself: a busy pane can swallow
        // the key, and only the host can see that it did and try again. Nothing
        // is lost by asking -- this target only exists when the host is alive.
        case WK_ENTER:
            return INTENT(KB_INTENT_WITHIN_OPEN);
        default:
            return NOTHING;
    }
}

// hunk's own keys. The ladder for left and right is hunk, annotated hunk, file.
static within_action_t resolve_hunk(within_key_t key, within_mod_t mod) {
    switch (key) {
        case WK_UP:
            return SENDS(mod == WM_WORD ? KC_U : mod == WM_LINE ? KC_B : KC_UP);
        case WK_DOWN:
            return SENDS(mod == WM_WORD ? KC_D : mod == WM_LINE ? KC_SPC : KC_DOWN);
        case WK_LEFT:
            return SENDS(mod == WM_WORD ? KC_LCBR : mod == WM_LINE ? KC_COMM : KC_LBRC);
        case WK_RIGHT:
            return SENDS(mod == WM_WORD ? KC_RCBR : mod == WM_LINE ? KC_DOT : KC_RBRC);
        case WK_SEARCH:
            return SENDS(KC_SLSH);
        case WK_NEXT:
            return SENDS(KC_N);
        case WK_PREV:
            return SENDS(S(KC_N));
        // Nothing for LEAVE: hunk has nothing WITHIN opened, and an Escape it did
        // not ask for is not worth sending on the chance it closes something.
        default:
            return NOTHING;
    }
}

within_action_t within_resolve(within_target_t target, within_key_t key, within_mod_t mod) {
    switch (target) {
        case WT_TRANSCRIPT:
            return resolve_transcript(key, mod);
        case WT_CLAUDE_SAFE:
            return resolve_claude_safe(key, mod);
        case WT_HUNK:
            return resolve_hunk(key, mod);
        default:
            return resolve_copy(key, mod);
    }
}
