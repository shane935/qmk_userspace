// Unit tests for tmux_context.c: the target resolution and the key table from
// docs/01-firmware.md. Run with test/run.sh.
//
// The table is the whole design of WITHIN mode, and most of its cells are a
// judgement about what is safe to send to a program that cannot be asked what
// state it is in. A wrong cell is not a crash -- it is Escape landing in a live
// Claude prompt and killing a turn. So the table is transcribed here as data and
// compared cell by cell, and the keys that must never be sent are asserted over
// every combination rather than spot-checked.

#include "../tmux_context.h"

#include <stdio.h>
#include <string.h>

#include "quantum_keycodes.h"

static int failures;

static void check_eq(unsigned got, unsigned want, const char *what) {
    if (got != want) {
        printf("FAIL %s: got 0x%04X, want 0x%04X\n", what, got, want);
        failures++;
    }
}

static void check(bool ok, const char *what) {
    if (!ok) {
        printf("FAIL %s\n", what);
        failures++;
    }
}

// ---------------------------------------------------------------- target

static kb_context_t ctx_of(uint8_t program, uint8_t transcript, uint8_t bits) {
    kb_context_t c;
    memset(&c, 0, sizeof(c));
    c.program    = program;
    c.transcript = transcript;
    c.tmux_bits  = bits;
    return c;
}

static void test_target(void) {
    const uint8_t in_tmux = KB_TMUX_PRESENT;
    const uint8_t copying = KB_TMUX_PRESENT | KB_TMUX_COPY_MODE;

    kb_context_t claude_open = ctx_of(KB_PROGRAM_CLAUDE, KB_TRANSCRIPT_OPEN, in_tmux);
    check_eq(within_target(&claude_open, true), WT_TRANSCRIPT, "claude with the viewer open");

    kb_context_t claude_shut = ctx_of(KB_PROGRAM_CLAUDE, KB_TRANSCRIPT_CLOSED, in_tmux);
    check_eq(within_target(&claude_shut, true), WT_CLAUDE_SAFE, "claude with the viewer closed");

    kb_context_t claude_dunno = ctx_of(KB_PROGRAM_CLAUDE, KB_TRANSCRIPT_UNKNOWN, in_tmux);
    check_eq(within_target(&claude_dunno, true), WT_CLAUDE_SAFE, "claude, viewer not yet observed");

    kb_context_t hunk = ctx_of(KB_PROGRAM_HUNK, KB_TRANSCRIPT_UNKNOWN, in_tmux);
    check_eq(within_target(&hunk, true), WT_HUNK, "hunk");

    kb_context_t shell = ctx_of(KB_PROGRAM_SHELL, KB_TRANSCRIPT_UNKNOWN, in_tmux);
    check_eq(within_target(&shell, true), WT_COPY, "a shell");

    kb_context_t other = ctx_of(KB_PROGRAM_OTHER, KB_TRANSCRIPT_UNKNOWN, in_tmux);
    check_eq(within_target(&other, true), WT_COPY, "some other program");

    // Precedence. Copy mode wins over the program underneath it, and a dead host
    // wins over everything -- including a report that is still sitting in ctx
    // from before the cable was pulled.
    kb_context_t claude_copying = ctx_of(KB_PROGRAM_CLAUDE, KB_TRANSCRIPT_OPEN, copying);
    check_eq(within_target(&claude_copying, true), WT_COPY, "copy mode beats an open viewer");

    check_eq(within_target(&claude_open, false), WT_COPY, "a dead host beats an open viewer");
    check_eq(within_target(&hunk, false), WT_COPY, "a dead host beats hunk");

    kb_context_t no_tmux = ctx_of(KB_PROGRAM_CLAUDE, KB_TRANSCRIPT_OPEN, 0);
    check_eq(within_target(&no_tmux, true), WT_COPY, "no tmux beats an open viewer");
}

// ---------------------------------------------------------------- key table

// docs/01-firmware.md, transcribed. `then` of KC_NO means a single keycode and
// an intent of 0 means it is keys rather than an intent; a row of all three zero
// is a cell that deliberately sends nothing.
static const struct {
    within_target_t target;
    within_key_t    key;
    within_mod_t    mod;
    uint16_t        want_key;
    uint16_t        want_then;
    uint8_t         want_intent;
    const char     *what;
} table[] = {
    // copy -- tmux's copy-mode-vi, as the COPY layer has always sent
    {WT_COPY, WK_UP, WM_NONE, KC_UP, KC_NO, 0, "copy up"},
    {WT_COPY, WK_UP, WM_WORD, KC_UP, KC_NO, 0, "copy up, word leaves it alone"},
    {WT_COPY, WK_UP, WM_LINE, KC_PGUP, KC_NO, 0, "copy up, line is a page"},
    {WT_COPY, WK_DOWN, WM_NONE, KC_DOWN, KC_NO, 0, "copy down"},
    {WT_COPY, WK_DOWN, WM_WORD, KC_DOWN, KC_NO, 0, "copy down, word leaves it alone"},
    {WT_COPY, WK_DOWN, WM_LINE, KC_PGDN, KC_NO, 0, "copy down, line is a page"},
    {WT_COPY, WK_LEFT, WM_NONE, KC_LEFT, KC_NO, 0, "copy left"},
    {WT_COPY, WK_LEFT, WM_WORD, KC_B, KC_NO, 0, "copy left, word is b"},
    {WT_COPY, WK_LEFT, WM_LINE, KC_0, KC_NO, 0, "copy left, line is 0"},
    {WT_COPY, WK_RIGHT, WM_NONE, KC_RGHT, KC_NO, 0, "copy right"},
    {WT_COPY, WK_RIGHT, WM_WORD, KC_W, KC_NO, 0, "copy right, word is w"},
    {WT_COPY, WK_RIGHT, WM_LINE, KC_DLR, KC_NO, 0, "copy right, line is $"},
    {WT_COPY, WK_SEARCH, WM_NONE, KC_SLSH, KC_NO, 0, "copy search"},
    {WT_COPY, WK_NEXT, WM_NONE, KC_N, KC_NO, 0, "copy next match"},
    {WT_COPY, WK_PREV, WM_NONE, S(KC_N), KC_NO, 0, "copy previous match"},
    {WT_COPY, WK_LEAVE, WM_NONE, KC_F20, KC_NO, 0, "copy leave is copy-mode -q"},
    {WT_COPY, WK_COPY, WM_NONE, KC_ENT, KC_NO, 0, "copy yank"},
    {WT_COPY, WK_PASTE, WM_NONE, KC_F20, C(KC_F22), 0, "copy paste leaves then pastes"},
    {WT_COPY, WK_ENTER, WM_NONE, KC_F19, KC_NO, 0, "entering copy opens copy mode"},
    {WT_COPY, WK_BKGD, WM_NONE, KC_NO, KC_NO, 0, "copy has nothing to background"},
    {WT_COPY, WK_DEEP, WM_NONE, KC_NO, KC_NO, 0, "deep is inert outside claude"},

    // transcript -- the viewer is open and the unit is the prompt
    {WT_TRANSCRIPT, WK_UP, WM_NONE, KC_UP, KC_NO, 0, "transcript up a line"},
    {WT_TRANSCRIPT, WK_UP, WM_WORD, C(KC_U), KC_NO, 0, "transcript up half a page"},
    {WT_TRANSCRIPT, WK_UP, WM_LINE, KC_B, KC_NO, 0, "transcript up a page"},
    {WT_TRANSCRIPT, WK_DOWN, WM_NONE, KC_DOWN, KC_NO, 0, "transcript down a line"},
    {WT_TRANSCRIPT, WK_DOWN, WM_WORD, C(KC_D), KC_NO, 0, "transcript down half a page"},
    {WT_TRANSCRIPT, WK_DOWN, WM_LINE, KC_SPC, KC_NO, 0, "transcript down a page"},
    {WT_TRANSCRIPT, WK_LEFT, WM_NONE, KC_LCBR, KC_NO, 0, "transcript previous prompt"},
    {WT_TRANSCRIPT, WK_LEFT, WM_WORD, KC_LCBR, KC_NO, 0, "transcript prompt, word is the same"},
    {WT_TRANSCRIPT, WK_LEFT, WM_LINE, KC_LCBR, KC_NO, 0, "transcript prompt, line is the same"},
    {WT_TRANSCRIPT, WK_RIGHT, WM_NONE, KC_RCBR, KC_NO, 0, "transcript next prompt"},
    {WT_TRANSCRIPT, WK_RIGHT, WM_LINE, KC_RCBR, KC_NO, 0, "transcript next prompt, line is the same"},
    {WT_TRANSCRIPT, WK_SEARCH, WM_NONE, KC_SLSH, KC_NO, 0, "transcript search"},
    {WT_TRANSCRIPT, WK_NEXT, WM_NONE, KC_N, KC_NO, 0, "transcript next match"},
    {WT_TRANSCRIPT, WK_PREV, WM_NONE, S(KC_N), KC_NO, 0, "transcript previous match"},
    {WT_TRANSCRIPT, WK_LEAVE, WM_NONE, C(KC_O), KC_NO, 0, "transcript leave closes the viewer"},
    {WT_TRANSCRIPT, WK_BKGD, WM_NONE, C(KC_F23), KC_NO, 0, "transcript background is a tmux key"},
    {WT_TRANSCRIPT, WK_DEEP, WM_NONE, KC_NO, KC_NO, KB_INTENT_WITHIN_DEEP, "deep is an intent here"},
    {WT_TRANSCRIPT, WK_COPY, WM_NONE, KC_NO, KC_NO, 0, "nothing to yank in the viewer"},
    {WT_TRANSCRIPT, WK_PASTE, WM_NONE, KC_NO, KC_NO, 0, "nothing to paste in the viewer"},
    {WT_TRANSCRIPT, WK_ENTER, WM_NONE, KC_NO, KC_NO, 0, "the viewer is already navigable"},

    // claude-safe -- the viewer is not confirmed open, so a live prompt is possible
    {WT_CLAUDE_SAFE, WK_UP, WM_NONE, KC_UP, KC_NO, 0, "claude-safe up moves the cursor"},
    {WT_CLAUDE_SAFE, WK_UP, WM_WORD, KC_PGUP, KC_NO, 0, "claude-safe up, word is PgUp"},
    {WT_CLAUDE_SAFE, WK_UP, WM_LINE, KC_PGUP, KC_NO, 0, "claude-safe up, line is PgUp too"},
    {WT_CLAUDE_SAFE, WK_DOWN, WM_NONE, KC_DOWN, KC_NO, 0, "claude-safe down moves the cursor"},
    {WT_CLAUDE_SAFE, WK_DOWN, WM_WORD, KC_PGDN, KC_NO, 0, "claude-safe down, word is PgDn"},
    {WT_CLAUDE_SAFE, WK_DOWN, WM_LINE, KC_PGDN, KC_NO, 0, "claude-safe down, line is PgDn too"},
    {WT_CLAUDE_SAFE, WK_LEFT, WM_NONE, KC_NO, KC_NO, 0, "claude-safe left sends nothing"},
    {WT_CLAUDE_SAFE, WK_RIGHT, WM_NONE, KC_NO, KC_NO, 0, "claude-safe right sends nothing"},
    {WT_CLAUDE_SAFE, WK_SEARCH, WM_NONE, KC_NO, KC_NO, 0, "claude-safe search would type a slash"},
    {WT_CLAUDE_SAFE, WK_NEXT, WM_NONE, KC_NO, KC_NO, 0, "claude-safe next would type an n"},
    {WT_CLAUDE_SAFE, WK_PREV, WM_NONE, KC_NO, KC_NO, 0, "claude-safe previous would type an N"},
    {WT_CLAUDE_SAFE, WK_LEAVE, WM_NONE, KC_NO, KC_NO, 0, "claude-safe has nothing to leave"},
    {WT_CLAUDE_SAFE, WK_BKGD, WM_NONE, C(KC_F23), KC_NO, 0, "claude-safe background is a tmux key"},
    {WT_CLAUDE_SAFE, WK_ENTER, WM_NONE, C(KC_O), KC_NO, 0, "entering claude opens the viewer"},
    {WT_CLAUDE_SAFE, WK_DEEP, WM_NONE, KC_NO, KC_NO, 0, "deep needs the viewer confirmed open"},

    // hunk -- the ladder is hunk, annotated hunk, file
    {WT_HUNK, WK_UP, WM_NONE, KC_UP, KC_NO, 0, "hunk up a line"},
    {WT_HUNK, WK_UP, WM_WORD, KC_U, KC_NO, 0, "hunk up half a page"},
    {WT_HUNK, WK_UP, WM_LINE, KC_B, KC_NO, 0, "hunk up a page"},
    {WT_HUNK, WK_DOWN, WM_NONE, KC_DOWN, KC_NO, 0, "hunk down a line"},
    {WT_HUNK, WK_DOWN, WM_WORD, KC_D, KC_NO, 0, "hunk down half a page"},
    {WT_HUNK, WK_DOWN, WM_LINE, KC_SPC, KC_NO, 0, "hunk down a page"},
    {WT_HUNK, WK_LEFT, WM_NONE, KC_LBRC, KC_NO, 0, "hunk previous hunk"},
    {WT_HUNK, WK_LEFT, WM_WORD, KC_LCBR, KC_NO, 0, "hunk previous annotated hunk"},
    {WT_HUNK, WK_LEFT, WM_LINE, KC_COMM, KC_NO, 0, "hunk previous file"},
    {WT_HUNK, WK_RIGHT, WM_NONE, KC_RBRC, KC_NO, 0, "hunk next hunk"},
    {WT_HUNK, WK_RIGHT, WM_WORD, KC_RCBR, KC_NO, 0, "hunk next annotated hunk"},
    {WT_HUNK, WK_RIGHT, WM_LINE, KC_DOT, KC_NO, 0, "hunk next file"},
    {WT_HUNK, WK_SEARCH, WM_NONE, KC_SLSH, KC_NO, 0, "hunk search"},
    {WT_HUNK, WK_NEXT, WM_NONE, KC_N, KC_NO, 0, "hunk next match"},
    {WT_HUNK, WK_PREV, WM_NONE, S(KC_N), KC_NO, 0, "hunk previous match"},
    {WT_HUNK, WK_LEAVE, WM_NONE, KC_ESC, KC_NO, 0, "hunk leave is Escape, never q"},
    {WT_HUNK, WK_BKGD, WM_NONE, KC_NO, KC_NO, 0, "hunk has nothing to background"},
    {WT_HUNK, WK_ENTER, WM_NONE, KC_NO, KC_NO, 0, "hunk is already navigable"},
    {WT_HUNK, WK_DEEP, WM_NONE, KC_NO, KC_NO, 0, "deep is inert in hunk"},
};

static void test_table(void) {
    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        within_action_t a = within_resolve(table[i].target, table[i].key, table[i].mod);
        char            what[96];

        snprintf(what, sizeof(what), "%s (key)", table[i].what);
        check_eq(a.key, table[i].want_key, what);
        snprintf(what, sizeof(what), "%s (second key)", table[i].what);
        check_eq(a.then, table[i].want_then, what);
        snprintf(what, sizeof(what), "%s (intent)", table[i].what);
        check_eq(a.intent, table[i].want_intent, what);
    }
}

// ---------------------------------------------------------------- invariants

// docs/01-firmware.md names four keystrokes that must never reach any program,
// "by construction of the table". Construction is exactly what a spot check
// cannot confirm, so this walks every cell there is.
static void test_never_sent(void) {
    for (int t = 0; t <= WT_HUNK; t++) {
        for (int k = 0; k < WK__COUNT; k++) {
            for (int m = 0; m < WM__COUNT; m++) {
                within_action_t a = within_resolve((within_target_t)t, (within_key_t)k, (within_mod_t)m);
                char            what[96];
                const uint16_t  sent[2] = {a.key, a.then};

                // q quits hunk, and no target has any use for it. Ctrl-B is
                // tmux's default prefix, and nothing here sends it at all --
                // Claude's C-x C-b goes through a tmux send-keys row instead,
                // which is the only reason this can be asserted everywhere.
                for (int s = 0; s < 2; s++) {
                    snprintf(what, sizeof(what), "target %d key %d mod %d never sends q", t, k, m);
                    check(sent[s] != KC_Q, what);
                    snprintf(what, sizeof(what), "target %d key %d mod %d never sends Ctrl-B", t, k, m);
                    check(sent[s] != C(KC_B), what);
                }

                // Escape interrupts a running Claude turn, so it stays off both
                // Claude targets -- the viewer is closed with Ctrl-O instead.
                // hunk takes Escape deliberately, which is why this is not global.
                if (t == WT_TRANSCRIPT || t == WT_CLAUDE_SAFE) {
                    for (int s = 0; s < 2; s++) {
                        snprintf(what, sizeof(what), "claude target %d key %d mod %d never sends Escape", t, k, m);
                        check(sent[s] != KC_ESC, what);
                    }
                }

                // Ctrl-D exits Claude Code at the prompt, so it is barred from
                // the target where the prompt may be live. In the viewer the
                // same chord is half a page down and the prompt is not reading,
                // which is the whole reason claude-safe exists as a separate
                // target rather than the two sharing one column.
                if (t == WT_CLAUDE_SAFE) {
                    for (int s = 0; s < 2; s++) {
                        snprintf(what, sizeof(what), "claude-safe key %d mod %d never sends Ctrl-D", k, m);
                        check(sent[s] != C(KC_D), what);
                        snprintf(what, sizeof(what), "claude-safe key %d mod %d never sends Ctrl-U", k, m);
                        check(sent[s] != C(KC_U), what);
                    }
                }
            }
        }
    }
}

// An action is keys or an intent, never both: the keymap sends one or declares
// the other, and a cell that did both would do something twice.
static void test_never_both(void) {
    for (int t = 0; t <= WT_HUNK; t++) {
        for (int k = 0; k < WK__COUNT; k++) {
            for (int m = 0; m < WM__COUNT; m++) {
                within_action_t a = within_resolve((within_target_t)t, (within_key_t)k, (within_mod_t)m);
                char            what[96];
                snprintf(what, sizeof(what), "target %d key %d mod %d is keys or an intent", t, k, m);
                check(!(a.intent != 0 && a.key != KC_NO), what);
                snprintf(what, sizeof(what), "target %d key %d mod %d has no orphan second key", t, k, m);
                check(!(a.then != KC_NO && a.key == KC_NO), what);
            }
        }
    }
}

int main(void) {
    test_target();
    test_table();
    test_never_sent();
    test_never_both();

    if (failures) {
        printf("%d failure(s)\n", failures);
        return 1;
    }
    printf("tmux_context: all checks passed\n");
    return 0;
}
