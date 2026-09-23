// Turns the raw token grid from parse.mjs into the key objects the page renders.
//
// Three kinds of work happen here, and the last two are the reason this file
// exists at all -- they are facts about the keymap that would otherwise have to
// be written out by hand and kept in sync:
//
//   1. decoding  -- LCTL_T(KC_S) becomes tap S / hold Control
//   2. transparency resolution -- _______ becomes what it actually fires
//   3. derivation -- which keys differ between Mac and Linux, and how you
//      reach each layer, computed by diffing and scanning rather than listed

import { lookup, modLabel, MOD_WRAPPERS, LAYER_HEADS } from './keycodes.mjs';

class DecorateError extends Error {
    constructor(message) {
        super(message);
        this.name = 'DecorateError';
    }
}

/** Splits `HEAD(a,b)` into its head and top-level arguments, or null. */
function callParts(token) {
    const open = token.indexOf('(');
    if (open === -1 || !token.endsWith(')')) return null;

    const head = token.slice(0, open);
    const inner = token.slice(open + 1, -1);
    const args = [];
    let depth = 0;
    let start = 0;
    for (let i = 0; i < inner.length; i++) {
        if (inner[i] === '(') depth++;
        else if (inner[i] === ')') depth--;
        else if (inner[i] === ',' && depth === 0) {
            args.push(inner.slice(start, i));
            start = i + 1;
        }
    }
    args.push(inner.slice(start));
    return { head, args: args.map((a) => a.trim()) };
}

/** Short display name for a layer: _NUM_MAC -> NUM. */
function layerLabel(name, groups) {
    for (const group of groups) {
        if (group.mac === name || group.linux === name || group.shared === name) {
            return group.short;
        }
    }
    return name.replace(/^_/, '');
}

/**
 * Decodes one token into {tap, hold, cat, flags}.
 * `where` is a human-readable location used only in error messages.
 */
function decode(token, { os, groups, customKeycodes, where }) {
    const plain = lookup(token, os);
    if (plain) return { tap: { label: plain.label, desc: plain.desc }, hold: null, cat: plain.cat, flags: [] };

    if (token === 'XXXXXXX' || token === 'KC_NO') {
        return { tap: null, hold: null, cat: 'blocked', flags: [] };
    }
    if (token === 'SC_LSPO' || token === 'SC_RSPC') {
        const side = token === 'SC_LSPO' ? 'Left' : 'Right';
        const bracket = token === 'SC_LSPO' ? '(' : ')';
        const { label, name } = modLabel('SFT', os);
        return {
            tap: { label: bracket, desc: `${bracket === '(' ? 'Open' : 'Close'} parenthesis` },
            hold: { label, desc: `${side} ${name}` },
            cat: 'space-cadet',
            flags: [],
        };
    }
    if (customKeycodes.includes(token)) {
        return { tap: { label: token.replace(/_/g, ' '), desc: token }, hold: null, cat: 'custom', flags: [] };
    }

    const call = callParts(token);
    if (!call) {
        throw new DecorateError(
            `${where}: unknown keycode '${token}'. Add it to keycodes.mjs before rebuilding.`
        );
    }
    const { head, args } = call;

    // Mod-taps: LCTL_T(KC_S), RGUI_T(KC_J), ...
    const modTap = head.match(/^([LR])(CTL|ALT|GUI|SFT)_T$/);
    if (modTap) {
        const [, side, mod] = modTap;
        const inner = decode(args[0], { os, groups, customKeycodes, where });
        const { label, name } = modLabel(mod, os);
        return {
            tap: inner.tap,
            hold: { label, desc: `${side === 'L' ? 'Left' : 'Right'} ${name}` },
            cat: 'mod-tap',
            flags: [],
        };
    }

    // Mod wrappers: G(KC_Z), C(KC_TAB), ...
    if (MOD_WRAPPERS[head] && args.length === 1) {
        const inner = decode(args[0], { os, groups, customKeycodes, where });
        const { label, name } = modLabel(MOD_WRAPPERS[head], os);
        const innerLabel = inner.tap ? inner.tap.label : args[0];
        return {
            tap: { label: `${label}${innerLabel}`, desc: `${name} + ${innerLabel}` },
            hold: null,
            cat: 'mod-combo',
            flags: [],
        };
    }

    // Layer heads: LT(_NAV_MAC, KC_SPC), TG(_TMUX), ...
    if (LAYER_HEADS[head]) {
        const target = layerLabel(args[0], groups);
        if (head === 'LT') {
            const inner = decode(args[1], { os, groups, customKeycodes, where });
            return {
                tap: inner.tap,
                hold: { label: target, desc: `Hold for the ${target} layer` },
                cat: 'layer-tap',
                flags: [],
                target: args[0],
            };
        }
        if (head === 'TG' || head === 'TO' || head === 'TT' || head === 'OSL') {
            return {
                tap: { label: target, desc: LAYER_HEADS[head].replace('{layer}', `the ${target} layer`) },
                hold: null,
                cat: 'layer-toggle',
                flags: [],
                target: args[0],
            };
        }
        // MO
        return {
            tap: null,
            hold: { label: target, desc: `Hold for the ${target} layer` },
            cat: 'layer-toggle',
            flags: [],
            target: args[0],
        };
    }

    throw new DecorateError(
        `${where}: unknown keycode head '${head}' in '${token}'. Teach decorate.mjs about it.`
    );
}

/** Decodes a whole layer, leaving _______ for resolveTransparent to fill in. */
function decodeLayer(layer, context) {
    return layer.expanded.map((token, index) => {
        const where = `[${layer.name}] key ${index}`;
        if (token === '_______' || token === 'KC_TRNS') {
            return { i: index, src: layer.tokens[index], expanded: token, cat: 'transparent' };
        }
        const decoded = decode(token, { ...context, where });
        return {
            i: index,
            src: layer.tokens[index],
            expanded: token === layer.tokens[index] ? null : token,
            ...decoded,
        };
    });
}

/**
 * Fills in a transparent key from the layer below.
 *
 * Showing a literal KC_TRNS would under-report: the two number-layer thumbs and
 * the two nav-layer thumbs are transparent precisely so the layer key you are
 * already holding keeps working, and that is worth seeing on the page.
 */
function resolveTransparent(keys, fallbacks) {
    return keys.map((key) => {
        if (key.cat !== 'transparent') return key;

        const resolutions = fallbacks.map(({ layer, keys: base }) => ({ layer, key: base[key.i] }));
        const distinct = new Set(resolutions.map((r) => JSON.stringify([r.key.tap, r.key.hold])));
        if (distinct.size !== 1) {
            throw new DecorateError(
                `key ${key.i} is transparent on a shared layer but resolves differently per OS: ` +
                resolutions.map((r) => `${r.layer}=${r.key.src}`).join(', ')
            );
        }

        const from = resolutions[0];
        return {
            ...key,
            tap: from.key.tap,
            hold: from.key.hold,
            resolvedFrom: { layer: from.layer, src: from.key.src, cat: from.key.cat },
        };
    });
}

/** Scans every layer for keys that lead to `target`. */
function accessPaths(layers, target, decodedByLayer, groups) {
    const paths = [];
    for (const layer of layers) {
        decodedByLayer[layer.name].forEach((key) => {
            if (key.target !== target) return;
            paths.push({
                from: layer.name,
                fromLabel: layerLabel(layer.name, groups),
                key: key.i,
                via: key.cat === 'layer-tap' ? 'hold' : 'tap',
                label: key.cat === 'layer-tap' ? key.tap?.label : key.tap?.label,
            });
        });
    }
    return paths;
}

/**
 * Builds the full data payload for the page.
 *
 * @param {object} parsed output of parseKeymap
 * @param {object} annotations hand-written prose and group definitions
 */
export function decorate(parsed, annotations) {
    const { groups } = annotations;
    const byLayer = {};

    // Pass one: decode every layer under its own OS, so G() reads as Command on
    // Mac and Super on Linux.
    for (const layer of parsed.layers) {
        const group = groups.find(
            (g) => g.mac === layer.name || g.linux === layer.name || g.shared === layer.name
        );
        if (!group) {
            throw new DecorateError(
                `layer ${layer.name} is not claimed by any group in annotations.mjs`
            );
        }
        const os = group.linux === layer.name ? 'linux' : 'mac';
        byLayer[layer.name] = decodeLayer(layer, {
            os,
            groups,
            customKeycodes: parsed.customKeycodes,
        });
    }

    // Pass two: resolve transparency against the base layers.
    const base = groups.find((g) => g.id === annotations.baseGroup);
    if (!base) throw new DecorateError(`baseGroup '${annotations.baseGroup}' is not a group`);

    for (const group of groups) {
        if (group.id === base.id) continue;
        for (const [os, layerName] of Object.entries(variantsOf(group))) {
            const fallbacks = group.shared
                ? [
                      { layer: base.mac, keys: byLayer[base.mac] },
                      { layer: base.linux, keys: byLayer[base.linux] },
                  ]
                : [{ layer: base[os], keys: byLayer[base[os]] }];
            byLayer[layerName] = resolveTransparent(byLayer[layerName], fallbacks);
        }
    }

    // Pass three: derive what would otherwise be hand-maintained.
    const outGroups = groups.map((group) => {
        const variants = variantsOf(group);
        const layerNames = [...new Set(Object.values(variants))];

        const osDiff = group.shared
            ? []
            : parsed.layers
                  .find((l) => l.name === group.mac)
                  .expanded.map((token, i) =>
                      token === parsed.layers.find((l) => l.name === group.linux).expanded[i] ? null : i
                  )
                  .filter((i) => i !== null);

        const access = layerNames.flatMap((name) =>
            accessPaths(parsed.layers, name, byLayer, groups)
        );
        if (group.id !== base.id && access.length === 0) {
            throw new DecorateError(
                `no key anywhere reaches ${layerNames.join('/')} -- the ${group.id} layer is unreachable`
            );
        }

        return {
            id: group.id,
            title: group.title,
            short: group.short,
            shared: Boolean(group.shared),
            prose: annotations.groupProse[group.id] ?? null,
            osDiff,
            access,
            keys: Object.fromEntries(
                Object.entries(variants).map(([os, name]) => [
                    os,
                    { layer: name, keys: attachNotes(byLayer[name], group.id, annotations) },
                ])
            ),
        };
    });

    assertBehaviourCoverage(outGroups, annotations);
    return { groups: outGroups, meta: annotations.meta, chords: annotations.chords, behaviours: annotations.behaviours };
}

function variantsOf(group) {
    return group.shared ? { mac: group.shared, linux: group.shared } : { mac: group.mac, linux: group.linux };
}

function attachNotes(keys, groupId, annotations) {
    return keys.map((key) => ({
        ...key,
        note: annotations.keyNotes[`${groupId}:${key.i}`] ?? null,
    }));
}

/**
 * The page must never show a key whose behaviour lives in the C without saying
 * what that behaviour is. Custom keycodes, layer toggles and the space-cadet
 * shifts all qualify, so the build refuses if any of them lack a note.
 */
function assertBehaviourCoverage(outGroups, annotations) {
    const needsNote = new Set(['custom', 'layer-toggle', 'space-cadet']);
    const missing = [];

    for (const group of outGroups) {
        for (const [os, variant] of Object.entries(group.keys)) {
            for (const key of variant.keys) {
                if (needsNote.has(key.cat) && !key.note) {
                    missing.push(`${group.id}:${key.i} (${key.src}, on ${variant.layer})`);
                }
            }
        }
    }
    if (missing.length) {
        throw new DecorateError(
            'these keys do what they do because of code in keymap.c, so they need a ' +
            'keyNotes entry in annotations.mjs:\n  ' + [...new Set(missing)].join('\n  ')
        );
    }

    // And the reverse: a note pointing at a key that no longer exists is a sign
    // the keymap moved under us.
    const valid = new Set(
        outGroups.flatMap((g) =>
            Object.values(g.keys).flatMap((v) => v.keys.map((k) => `${g.id}:${k.i}`))
        )
    );
    for (const ref of Object.keys(annotations.keyNotes)) {
        if (!valid.has(ref)) {
            throw new DecorateError(`annotations.mjs has a keyNote for '${ref}', which is not a key`);
        }
    }
}

export { DecorateError };
