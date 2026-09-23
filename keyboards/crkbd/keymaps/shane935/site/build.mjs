#!/usr/bin/env node
// Builds the keymap reference page from keymap.c.
//
//   node build.mjs --dump    print the parsed keymap as JSON, build nothing
//
// All paths are resolved relative to this file, so the working directory
// never matters.

import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

import { parseKeymap, assertKeyCount, ParseError } from './parse.mjs';

const here = dirname(fileURLToPath(import.meta.url));

const KEYMAP_C = resolve(here, '../keymap.c');
const INFO_JSON = resolve(here, '../../../info.json');

/** Reads the crkbd layout table: 36 {matrix, x, y} entries, in keymap.c order. */
function readLayouts() {
    const info = JSON.parse(readFileSync(INFO_JSON, 'utf8'));
    if (!info.layouts) throw new ParseError(`${INFO_JSON} has no 'layouts' section`);
    return info.layouts;
}

function main(argv) {
    const layouts = readLayouts();
    const parsed = parseKeymap(KEYMAP_C, Object.keys(layouts));

    const geometry = layouts[parsed.layoutMacro].layout;
    assertKeyCount(parsed, geometry.length);

    if (argv.includes('--dump')) {
        process.stdout.write(JSON.stringify(parsed, null, 2) + '\n');
        return;
    }

    // Rendering lands in the next milestone.
    console.log(
        `parsed ${parsed.layers.length} layers of ${geometry.length} keys ` +
        `(${parsed.layoutMacro}) from keymap.c`
    );
    console.log('nothing to render yet -- run with --dump to inspect the grid');
}

try {
    main(process.argv.slice(2));
} catch (error) {
    if (error instanceof ParseError) {
        console.error(`keymap.c: ${error.message}`);
        process.exit(1);
    }
    throw error;
}
