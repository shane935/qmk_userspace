// The half of this keymap that is not in the keymaps[] array.
//
// Everything here is written by hand, because no parser can read it out of the
// C: OS_SWAP's effect lives in process_record_user, the Caps Word chord lives
// in config.h and has no array entry at all, and the tapping term is a QMK
// default that config.h leaves commented out. Keep it accurate by hand.
//
// A module rather than JSON so it can carry comments and multi-line prose.

export default {
    // Which layers pair up, and what to call them. decorate.mjs resolves
    // transparency against `baseGroup`.
    baseGroup: 'base',
    groups: [
        { id: 'base', title: 'Base', short: 'BASE', mac: '_MAC', linux: '_LINUX' },
        { id: 'num', title: 'Numbers', short: 'NUM', mac: '_NUM_MAC', linux: '_NUM_LINUX' },
        { id: 'nav', title: 'Nav', short: 'NAV', mac: '_NAV_MAC', linux: '_NAV_LINUX' },
        { id: 'tmux', title: 'Tmux', short: 'TMUX', shared: '_TMUX' },
    ],

    meta: {
        tappingTerm: 200,
        tappingTermNote:
            'QMK default — config.h leaves TAPPING_TERM commented out. Hold a home-row key ' +
            'past 200 ms and you get the modifier; release sooner and you get the letter.',
        tapFlavour:
            'No PERMISSIVE_HOLD, HOLD_ON_OTHER_KEY_PRESS or QUICK_TAP_TERM are set, so a fast ' +
            'roll across the home row resolves as taps rather than firing a modifier.',
        configFlags: ['SPLIT_LAYER_STATE_ENABLE', 'BOTH_SHIFTS_TURNS_ON_CAPS_WORD'],
        rules: ['OLED_ENABLE', 'CAPS_WORD_ENABLE'],
    },

    groupProse: {
        base: 'QWERTY with the outer pinky columns removed. The home row doubles as modifiers.',
        num: 'Numpad-style digits under the left hand, symbols down the right of it, and the ' +
            'right home row becomes bare modifiers so you can chord them with the digits.',
        nav: 'Arrows under the right index/middle/ring, editing keys below them, and bare ' +
            'modifiers on the left home row to combine with those arrows.',
        tmux: 'A toggle, not a hold. Almost every key is dead while it is on.',
    },

    // Keyed by "<group>:<key index>". decorate.mjs fails the build if a key
    // whose behaviour lives in the C has no entry here.
    keyNotes: {
        'nav:0': 'OS_SWAP. Flips the default layer between Mac and Linux and writes it to ' +
            'EEPROM, so it survives unplugging. This key is the only way to switch, and it ' +
            'exists only here on the Nav layer. The OLED shows which mode you are in.',
        'nav:34': 'Toggles the Tmux layer ON. It stays on until you tap this same key again ' +
            '— it is TG(), not a hold — and the OLED prints TMUX while it is active.',
        'tmux:34': 'Toggles the Tmux layer back OFF. This is the only live key that gets you ' +
            'out, so it is worth memorising its position.',
        'base:32': 'Space cadet: Shift when held, an opening parenthesis when tapped on its own.',
        'base:33': 'Space cadet: Shift when held, a closing parenthesis when tapped on its own.',
    },

    chords: [
        {
            title: 'Caps Word',
            keys: [32, 33],
            group: 'base',
            body: 'Press both thumb shifts together to enter Caps Word: the next word types in ' +
                'capitals and it switches itself off at the first space or punctuation. Set by ' +
                'BOTH_SHIFTS_TURNS_ON_CAPS_WORD in config.h — there is no entry for it in the ' +
                'keymap, so it is invisible in the source. The OLED prints CAPS while it is on.',
        },
    ],

    behaviours: [
        {
            title: 'The OLED',
            body: 'The master half prints four lines: the label OS, then MAC or LINUX, then ' +
                'TMUX when that layer is toggled on, then CAPS when Caps Word is active. ' +
                'Blank lines mean neither is on.',
        },
        {
            title: 'Why every layer exists twice',
            body: 'Mac and Linux differ in only three places: the order of the home-row ' +
                'modifiers, the Nav layer bottom row (Mac sends Command+Z/X/C/V/F, Linux sends ' +
                'the dedicated Undo/Cut/Copy/Paste/Find keys), and a Ctrl/Super swap on the ' +
                'Numbers home row. Use the "differs by OS" toggle to see exactly which keys.',
        },
    ],
};
