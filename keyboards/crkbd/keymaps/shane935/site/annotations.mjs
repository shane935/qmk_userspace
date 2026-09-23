// The half of this keymap that is not in the keymaps[] array.
//
// Everything here is written by hand, because no parser can read it out of the
// C: OS_SWAP's effect lives in process_record_user, the Caps Word chord lives
// in config.h and has no array entry at all, and the tapping term is a QMK
// default that config.h leaves commented out. Keep it accurate by hand.
//
// The tmux keycodes are the bulk of it. Every one of them is a case in
// process_record_user that sends a prefix key or a command down tmux's prompt,
// and the array shows only the name, so customKeys below is the only place the
// page learns what they do.
//
// A module rather than JSON so it can carry comments and multi-line prose.

// tmux mode is one state with four faces, so all four tabs say the same thing
// about it.
const TMUX_BANNER = {
    title: 'tmux mode is a toggle, not a hold.',
    body:
        'It stays on until Exit or Detach, and everything hatched below is dead while it is. ' +
        'The four mode keys and Exit live on the shared _TMUX layer underneath, which is why ' +
        'they show through as transparent on all four modes and sit on the same keys in each. ' +
        'The OLED prints which mode you are in, and which thumb modifier is live.',
};

export default {
    // Which layers pair up, and what to call them. decorate.mjs resolves
    // transparency against `baseGroup`, through `under` first where it is set.
    //
    // _TMUX has no tab of its own: it is never on without exactly one mode
    // layer above it, so a tab for it would be a state you cannot be in. It is
    // claimed by being stacked under all four modes instead.
    baseGroup: 'base',
    groups: [
        { id: 'base', title: 'Base', short: 'BASE', mac: '_MAC', linux: '_LINUX' },
        { id: 'num', title: 'Numbers', short: 'NUM', mac: '_NUM_MAC', linux: '_NUM_LINUX' },
        { id: 'nav', title: 'Nav', short: 'NAV', mac: '_NAV_MAC', linux: '_NAV_LINUX' },
        // Pane first: it is the mode tmux mode starts in.
        { id: 'pane', title: 'Tmux · Pane', short: 'PANE', shared: '_TMUX_PANE', under: ['_TMUX'] },
        { id: 'window', title: 'Tmux · Window', short: 'WINDOW', shared: '_TMUX_WINDOW', under: ['_TMUX'] },
        { id: 'tree', title: 'Tmux · Tree', short: 'TREE', shared: '_TMUX_TREE', under: ['_TMUX'] },
        { id: 'copy', title: 'Tmux · Copy', short: 'COPY', shared: '_TMUX_COPY', under: ['_TMUX'] },
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
        pane: 'Where tmux mode starts. The arrows move between panes, and the three left ' +
            'thumbs are held modifiers that turn those same arrows into resize, split and swap.',
        window: 'Left and right move between windows, up and down between sessions. The two ' +
            'left thumbs are held modifiers for making and moving windows.',
        tree: 'choose-tree, driven by its own key table, so most of this layer is plain keys ' +
            'sent straight through.',
        copy: 'tmux scrollback, driven by copy-mode-vi. The two left thumbs are toggles rather ' +
            'than holds, and they change what the arrows mean.',
    },

    groupBanner: {
        pane: TMUX_BANNER,
        window: TMUX_BANNER,
        tree: TMUX_BANNER,
        copy: TMUX_BANNER,
    },

    // Keyed by custom keycode, because the behaviour follows the keycode rather
    // than the position: TM_TREE means the same thing on all five tmux layers.
    // decorate.mjs fails the build on a custom keycode that is missing here, and
    // on an entry here for a keycode keymap.c no longer has.
    //
    // `hold` instead of `label` means the key is held rather than tapped.
    // `target` is the layer the key opens, and is what the page derives "how you
    // get to this layer" from -- so only the keys whose job is to open a layer
    // carry one.
    customKeys: {
        OS_SWAP: {
            label: 'OS',
            desc: 'Swap the default layer between Mac and Linux',
            note: 'Flips the default layer between Mac and Linux and writes it to EEPROM, so ' +
                'it survives unplugging. This key is the only way to switch, and it exists ' +
                'only here on the Nav layer. The OLED shows which mode you are in.',
        },

        // --- getting in and out of tmux mode -------------------------------
        TMUX_ON: {
            label: 'tmux',
            desc: 'Turn tmux mode on, in Pane mode',
            target: '_TMUX_PANE',
            note: 'Turns tmux mode on and lands you in Pane mode. It is a toggle, not a hold: ' +
                'tmux mode stays on until Exit or Detach, and while it is on almost every ' +
                'other key is dead.',
        },
        TM_TREE: {
            label: 'Tree',
            desc: "Tree mode — tmux's session and window tree",
            target: '_TMUX_TREE',
            note: 'Opens choose-tree -Zw -O activity (zoomed, windows included, most recently ' +
                'used first) and switches the keyboard to Tree mode. Tapping it again while ' +
                'already in Tree mode quits the tree and drops back to Pane.',
        },
        TM_WIN: {
            label: 'Window',
            desc: 'Window mode — move between windows and sessions',
            target: '_TMUX_WINDOW',
            note: 'Switches the keyboard to Window mode. Nothing is opened in tmux itself — ' +
                'Window mode only changes what the keys send. Pressing it when you are ' +
                'already in Window mode does nothing.',
        },
        TM_PANE: {
            label: 'Pane',
            desc: 'Pane mode — move, split and resize panes',
            target: '_TMUX_PANE',
            note: 'Switches the keyboard to Pane mode, the mode tmux mode starts in. Like ' +
                'Window mode it opens nothing; it only changes what the keys send.',
        },
        TM_COPY: {
            label: 'Copy',
            desc: "Copy mode — tmux's scrollback",
            target: '_TMUX_COPY',
            note: 'Sends prefix [ to put the pane into copy mode and switches the keyboard to ' +
                'Copy mode. Tapping it again sends q to leave copy mode and drops back to Pane.',
        },
        TM_EXIT: {
            label: 'Exit',
            desc: 'Leave tmux mode',
            note: 'Turns tmux mode off and gives the keyboard back. If a tree or copy mode is ' +
                'open it sends q first, so the next thing you type lands in the shell rather ' +
                'than in a mode that was left open. It is the only key that gets you out ' +
                'without touching tmux state, so it is worth memorising its position.',
        },
        TM_DTCH: {
            label: 'Detach',
            desc: 'Detach the client, then leave tmux mode',
            note: 'Sends prefix d to detach, and turns tmux mode off with it — there is no ' +
                'longer a session to send keys to.',
        },

        // --- Pane mode -----------------------------------------------------
        TP_UP: {
            label: '↑',
            desc: 'Focus the pane above',
            note: 'prefix ↑ on its own. Resize makes it resize-pane -U 5, Split makes it ' +
                'split-window -vb, and Move makes it swap-pane -s {up-of} — which takes the ' +
                'neighbour as -s rather than -t so focus ends up on the pane that moved, ' +
                'which is what lets repeated presses push the same pane along.',
        },
        TP_DOWN: {
            label: '↓',
            desc: 'Focus the pane below',
            note: 'prefix ↓ on its own; resize-pane -D 5 with Resize, split-window -v with ' +
                'Split, swap-pane -s {down-of} with Move.',
        },
        TP_LEFT: {
            label: '←',
            desc: 'Focus the pane to the left',
            note: 'prefix ← on its own; resize-pane -L 5 with Resize, split-window -hb with ' +
                'Split, swap-pane -s {left-of} with Move.',
        },
        TP_RGHT: {
            label: '→',
            desc: 'Focus the pane to the right',
            note: 'prefix → on its own; resize-pane -R 5 with Resize, split-window -h with ' +
                'Split, swap-pane -s {right-of} with Move.',
        },
        TP_LAST: {
            label: 'Last',
            desc: 'Back to the pane you were in before',
            note: 'prefix ; — tmux\'s last-pane.',
        },
        TP_ZOOM: {
            label: 'Zoom',
            desc: 'Zoom this pane in or out',
            note: 'prefix z. Zooming fills the window with this pane; tapping it again puts ' +
                'the others back.',
        },
        TP_LYT: {
            label: 'Layout',
            desc: 'Cycle the window layout',
            note: "prefix Space, which steps through tmux's preset layouts.",
        },
        TP_BRK: {
            label: 'Break',
            desc: 'Break this pane out into its own window',
            note: 'prefix ! — the pane leaves this window and becomes a window of its own.',
        },
        TP_RSZE: {
            hold: { label: 'Resize', desc: 'Held: the arrows resize the pane' },
            note: 'Held, not tapped. While it is down the four arrows send resize-pane in ' +
                '5-cell steps instead of moving the focus. Releasing it clears the modifier, ' +
                'and so does changing mode.',
        },
        TP_SPLT: {
            hold: { label: 'Split', desc: 'Held: the arrows split the pane' },
            note: 'Held. The arrows become split-window in that direction, each with ' +
                "-c '#{pane_current_path}' so the new pane opens where the current one is. " +
                'Up and left add -b, which puts the new pane before the current one.',
        },
        TP_MOVE: {
            hold: { label: 'Move', desc: 'Held: the arrows swap this pane with its neighbour' },
            note: 'Held. The arrows become swap-pane against the neighbour in that direction. ' +
                'Window mode has a Move on the same thumb and neither can be live while the ' +
                'other is, so the two share one state.',
        },

        // --- Window mode ---------------------------------------------------
        TW_UP: {
            label: '↑',
            desc: 'Previous session',
            note: 'prefix ( . Up and down move between sessions here; left and right move ' +
                'between windows. Does nothing while New or Move is held — those two only ' +
                'apply to windows.',
        },
        TW_DOWN: {
            label: '↓',
            desc: 'Next session',
            note: 'prefix ) . Does nothing while New or Move is held.',
        },
        TW_LEFT: {
            label: '←',
            desc: 'Previous window',
            note: 'prefix p on its own. With New held it is new-window -b, inserting a window ' +
                "before this one in the current pane's directory; with Move held it is " +
                'swap-window -d -t -1, where -d is what makes the client follow the window it ' +
                'moved.',
        },
        TW_RGHT: {
            label: '→',
            desc: 'Next window',
            note: 'prefix n on its own. With New held it is new-window -a, inserting a window ' +
                'after this one; with Move held it is swap-window -d -t +1.',
        },
        TW_LAST: {
            label: 'Last',
            desc: 'Back to the window you were in before',
            note: "prefix l — tmux's last-window.",
        },
        TW_NEW: {
            hold: { label: 'New', desc: 'Held: left and right make a window' },
            note: 'Held. Only left and right do anything while it is down: up and down are ' +
                'sessions, and opening a new session still needs prompt handling that is not ' +
                'written yet.',
        },
        TW_MOVE: {
            hold: { label: 'Move', desc: 'Held: left and right move this window' },
            note: "Held. Shares its state with Pane mode's Move on the same thumb; only one " +
                'of the two modes can be live at a time.',
        },

        // --- Tree mode -----------------------------------------------------
        TT_SEL: {
            label: 'Select',
            desc: 'Choose the highlighted item and close the tree',
            note: "Sends Enter, and what that means is up to the tree's own template. The " +
                'keyboard returns to Pane mode because the tree is now closed.',
        },
        TT_KILL: {
            label: 'Kill',
            desc: 'Kill the highlighted session or window — press twice',
            note: 'A bare x, because the tree reads keys itself rather than through the prefix. ' +
                'It kills whatever the cursor is on, session or window, after a second press to ' +
                'confirm; any other key answers no. The tree stays open afterwards. This is the ' +
                'only key on the keyboard that destroys anything, and the tree is deliberately ' +
                'the only place it exists: you have to open the tree and put the cursor on the ' +
                'thing before you can kill it.',
        },

        // --- Copy mode -----------------------------------------------------
        TC_UP: {
            label: '↑',
            desc: 'Up a line',
            note: 'Up a line on its own, Page Up with Line toggled on. Word leaves it alone — ' +
                'only Line changes all four arrows.',
        },
        TC_DOWN: {
            label: '↓',
            desc: 'Down a line',
            note: 'Down a line on its own, Page Down with Line toggled on.',
        },
        TC_LEFT: {
            label: '←',
            desc: 'Left a character',
            note: 'Left a character on its own; b (back a word) with Word on, 0 (start of the ' +
                'line) with Line on.',
        },
        TC_RGHT: {
            label: '→',
            desc: 'Right a character',
            note: 'Right a character on its own; w (forward a word) with Word on, $ (end of ' +
                'the line) with Line on.',
        },
        TC_COPY: {
            label: 'Copy',
            desc: 'Copy the selection and leave copy mode',
            note: 'Sends Enter, which in copy-mode-vi is copy-pipe-and-cancel, so the ' +
                'selection is copied and copy mode closes itself. The keyboard drops back to ' +
                'Pane mode with it.',
        },
        TC_PSTE: {
            label: 'Paste',
            desc: 'Paste the buffer and leave tmux mode',
            note: 'Sends q to leave copy mode, then prefix ] to paste the buffer, then turns ' +
                'tmux mode off — pasting is taken to be the last thing you wanted from tmux.',
        },
        TC_WORD: {
            label: 'Word',
            desc: 'Toggle: left and right move by word',
            note: 'A toggle rather than a hold, unlike the Pane and Window thumb modifiers. ' +
                'Tap it again, or change mode, to clear it. It affects only left and right.',
        },
        TC_LINE: {
            label: 'Line',
            desc: 'Toggle: the arrows move by line and page',
            note: 'A toggle, like Word. Left and right go to the start and end of the line, ' +
                'and up and down go a page at a time.',
        },
    },

    // Keyed by "<group>:<key index>", for the things that are about one
    // position rather than one keycode. Plain keycodes on the tree and copy
    // layers need these: they are ordinary keys as far as the keymap is
    // concerned, and only mean something because tmux is reading them.
    keyNotes: {
        'base:32': 'Space cadet: Shift when held, an opening parenthesis when tapped on its own.',
        'base:33': 'Space cadet: Shift when held, a closing parenthesis when tapped on its own.',

        'tree:6': 'Alt with minus folds every item in the tree. The bare - and + keys are only ' +
            'aliases for left and right, which the arrows already cover, so the modified pair ' +
            'is the only reason these two keys are here at all.',
        'tree:8': 'Alt with plus unfolds every item in the tree.',

        'copy:5': 'n in copy-mode-vi: jump to the next match of the last search. Copy mode is a ' +
            'real tmux mode, so unmodified keys go straight through as its own keys.',
        'copy:6': 'Space begins a selection at the cursor. Move with the arrows and then Copy.',
        'copy:8': 'V selects whole lines rather than characters.',
        'copy:9': 'Escape clears the selection without leaving copy mode. Copy and Paste both ' +
            'leave it, and so does tapping the Copy mode key again.',
        'copy:15': 'N: the previous match of the last search.',
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
            body: 'The master half prints the label OS, then MAC or LINUX, then the tmux mode ' +
                'and thumb modifier if tmux mode is on — TREE, WINDOW, PANE or COPY, followed ' +
                'by RESIZE, SPLIT, MOVE, NEW, WORD or LINE — then CAPS while Caps Word is ' +
                'active. Blank lines mean neither is on. The tmux layers are toggled rather ' +
                'than held, so this line is the only way to tell which mode you are in.',
        },
        {
            title: 'Every tmux key is prefix plus one key',
            body: 'The prefix is Ctrl-B, hard-coded in keymap.c, and it is the same on Mac and ' +
                'Linux — which is why the tmux layers are shared rather than duplicated per ' +
                'OS. Anything without a suitable default binding goes through tmux\'s command ' +
                'prompt instead, so none of it depends on your tmux.conf.',
        },
        {
            title: 'The thumb modifiers',
            body: 'Pane and Window put modifiers on the left thumbs and Copy puts two more ' +
                'there, and they are the part of this keymap you cannot see: they change what ' +
                'the arrows send without changing the arrows. Pane and Window hold theirs; ' +
                'Copy toggles its own. Only one can ever be live, and changing mode always ' +
                'clears it, so a Copy toggle cannot survive into Pane.',
        },
        {
            title: 'Killing takes two presses, and only from the tree',
            body: 'Kill is the one key here that destroys anything, so it is kept behind as many ' +
                'steps as possible: it exists only in Tree mode, which means opening the tree ' +
                'and moving the cursor onto the session or window first. Then it raises tmux\'s ' +
                'own "(y/n)" prompt rather than killing outright, and since the Tree layer has ' +
                'no y on it, a second press of the same key is what answers. While one is ' +
                'waiting the OLED shows KILL?. Pressing anything else answers no first and then ' +
                'does its own job, so the prompt can never outlive the press that raised it and ' +
                'swallow a later key. Pane and Window mode have no kill at all — closing a pane ' +
                'is still exit in the shell.',
        },
        {
            title: 'Why every layer exists twice',
            body: 'Mac and Linux differ in only three places: the order of the home-row ' +
                'modifiers, the Nav layer bottom row (Mac sends Command+Z/X/C/V/F, Linux sends ' +
                'the dedicated Undo/Cut/Copy/Paste/Find keys), and a Ctrl/Super swap on the ' +
                'Numbers home row. Use the "differs by OS" toggle to see exactly which keys. ' +
                'The five tmux layers are the exception — they are shared, because the tmux ' +
                'prefix is Ctrl on both.',
        },
    ],
};
