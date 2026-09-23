// Closed table of every keycode this keymap uses.
//
// Deliberately closed: decorate.mjs throws on anything not listed here. Adding
// a new keycode to keymap.c breaks the build until you give it a label, which
// is cheaper and stricter than a test and is the main thing keeping the page
// honest.

// GUI and its wrapper render differently per OS, so mods are resolved late.
export const MODS = {
    CTL: { mac: ['⌃', 'Control'], linux: ['Ctrl', 'Control'] },
    ALT: { mac: ['⌥', 'Option'], linux: ['Alt', 'Alt'] },
    GUI: { mac: ['⌘', 'Command'], linux: ['Super', 'Super'] },
    SFT: { mac: ['⇧', 'Shift'], linux: ['Shift', 'Shift'] },
};

// Single-letter mod wrappers, e.g. G(KC_Z).
export const MOD_WRAPPERS = { C: 'CTL', A: 'ALT', G: 'GUI', S: 'SFT' };

// Heads that take a layer as their first argument.
export const LAYER_HEADS = {
    LT: 'hold for {layer}, tap for {tap}',
    MO: 'hold for {layer}',
    TG: 'toggle {layer} on or off',
    TO: 'switch to {layer}',
    TT: 'hold for {layer}; tap repeatedly to lock it on',
    OSL: 'next key comes from {layer}',
};

const letter = (c) => [c, c, 'letter'];
const digit = (d) => [d, d, 'number'];
const punct = (label, desc) => [label, desc, 'punct'];
const nav = (label, desc) => [label, desc, 'nav'];
const space = (label, desc) => [label, desc, 'whitespace'];
const edit = (label, desc) => [label, desc, 'editing'];

// token -> [display label, longer description, category]
const TABLE = {
    ...Object.fromEntries(
        'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('').map((c) => [`KC_${c}`, letter(c)])
    ),
    ...Object.fromEntries('0123456789'.split('').map((d) => [`KC_${d}`, digit(d)])),

    KC_SCLN: punct(';', 'Semicolon — shifted, colon'),
    KC_QUOT: punct("'", 'Apostrophe — shifted, double quote'),
    KC_COMM: punct(',', 'Comma — shifted, less-than'),
    KC_DOT: punct('.', 'Full stop — shifted, greater-than'),
    KC_SLSH: punct('/', 'Slash — shifted, question mark'),
    KC_BSLS: punct('\\', 'Backslash — shifted, pipe'),
    KC_MINS: punct('-', 'Minus — shifted, underscore'),
    KC_EQL: punct('=', 'Equals — shifted, plus'),
    KC_GRV: punct('`', 'Backtick — shifted, tilde'),
    KC_LBRC: punct('[', 'Left bracket — shifted, left brace'),
    KC_RBRC: punct(']', 'Right bracket — shifted, right brace'),

    KC_SPC: space('Space', 'Space'),
    KC_ENT: space('Enter', 'Enter'),
    KC_BSPC: space('Bksp', 'Backspace'),
    KC_DEL: space('Del', 'Forward delete'),

    KC_TAB: nav('Tab', 'Tab'),
    KC_ESC: nav('Esc', 'Escape'),
    KC_LEFT: nav('←', 'Left arrow'),
    KC_DOWN: nav('↓', 'Down arrow'),
    KC_UP: nav('↑', 'Up arrow'),
    KC_RGHT: nav('→', 'Right arrow'),
    KC_PGUP: nav('PgUp', 'Page up'),
    KC_PGDN: nav('PgDn', 'Page down'),

    KC_UNDO: edit('Undo', 'Undo — the dedicated key, not Ctrl-Z'),
    KC_CUT: edit('Cut', 'Cut — the dedicated key, not Ctrl-X'),
    KC_COPY: edit('Copy', 'Copy — the dedicated key, not Ctrl-C'),
    KC_PASTE: edit('Paste', 'Paste — the dedicated key, not Ctrl-V'),
    KC_FIND: edit('Find', 'Find — the dedicated key, not Ctrl-F'),
};

// Plain modifier keycodes, resolved per OS like the wrappers are.
const PLAIN_MODS = {
    KC_LCTL: ['CTL', 'Left'], KC_RCTL: ['CTL', 'Right'],
    KC_LALT: ['ALT', 'Left'], KC_RALT: ['ALT', 'Right'],
    KC_LGUI: ['GUI', 'Left'], KC_RGUI: ['GUI', 'Right'],
    KC_LSFT: ['SFT', 'Left'], KC_RSFT: ['SFT', 'Right'],
};

/** Resolves a mod's symbol and name for one OS. */
export function modLabel(mod, os) {
    const entry = MODS[mod];
    if (!entry) throw new Error(`unknown modifier '${mod}'`);
    const [label, name] = entry[os];
    return { label, name };
}

/**
 * Looks up a plain keycode. Returns null if it isn't in the table, so the
 * caller can raise an error that names the layer and key index.
 */
export function lookup(token, os) {
    if (PLAIN_MODS[token]) {
        const [mod, side] = PLAIN_MODS[token];
        const { label, name } = modLabel(mod, os);
        return { label, desc: `${side} ${name}`, cat: 'modifier' };
    }
    const entry = TABLE[token];
    if (!entry) return null;
    const [label, desc, cat] = entry;
    return { label, desc, cat };
}
