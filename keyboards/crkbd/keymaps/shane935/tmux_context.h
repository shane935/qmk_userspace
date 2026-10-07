// What WITHIN mode sends, resolved from the host's report.
//
// Pure functions of the context and the held thumb: no QMK state, no layer
// stack, nothing remembered. The keymap calls these on every keypress rather
// than caching the answer, because the pane under the cursor can change without
// the keyboard being told. Built natively by test/run.sh, which is where the
// table in docs/01-firmware.md is actually checked.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "kb_protocol.h"

// What WITHIN is driving. Resolved in this order on every keypress.
typedef enum {
    // tmux's own copy mode. Also the answer whenever the host is silent: with
    // no report there is no program knowledge, and copy mode is the one thing
    // that works anyway.
    WT_COPY,
    // Claude's transcript viewer, observed open by the host.
    WT_TRANSCRIPT,
    // A Claude pane whose viewer is not confirmed open. The careful target:
    // anything that would reach a live prompt sends nothing instead.
    WT_CLAUDE_SAFE,
    WT_HUNK,
} within_target_t;

// One per key on the WITHIN layer, plus the pseudo-key for arriving there.
typedef enum {
    WK_UP,
    WK_DOWN,
    WK_LEFT,
    WK_RIGHT,
    WK_SEARCH,
    WK_NEXT,
    WK_PREV,
    WK_LEAVE,
    WK_BKGD,
    WK_SELECT,
    WK_COPY,
    WK_PASTE,
    // Not a key on the layer: what pressing TM_WITHIN has to do to make the
    // target navigable. It escalates on a Claude pane -- the first press asks
    // the host to open the viewer, a second asks for the whole conversation in
    // copy mode -- so neither of those needs a key of its own.
    WK_ENTER,
    WK__COUNT,
} within_key_t;

// The held thumb. Only these three mean anything inside WITHIN.
typedef enum { WM_NONE, WM_WORD, WM_LINE, WM__COUNT } within_mod_t;

// What a key does: a keycode, optionally a second one sent straight after it, or
// an intent for the host. All of it zero means the key sends nothing, which is a
// real answer rather than a gap -- it is how WT_CLAUDE_SAFE refuses to put
// Escape into a live Claude prompt.
//
// A `key` whose basic keycode is KC_F13..KC_F24, with or without a modifier on
// it, is a row of the tmux key table: the keymap sends those with tmux_fkey so
// the host hears the event, rather than tapping them directly.
typedef struct {
    uint16_t key;
    uint16_t then;
    uint8_t  intent;
} within_action_t;

within_target_t within_target(const kb_context_t *ctx, bool host_alive);
within_action_t within_resolve(within_target_t target, within_key_t key, within_mod_t mod);
