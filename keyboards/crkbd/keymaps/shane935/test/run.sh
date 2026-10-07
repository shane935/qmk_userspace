#!/bin/sh
# Builds and runs the kb_protocol unit test. kb_protocol.c includes no QMK
# headers, which is the whole reason it is a separate file, so this needs a C
# compiler and nothing else -- no QMK checkout, no toolchain, no keyboard.
#
# With a native compiler it just builds and runs. Without one, set WASI_SDK to
# an unpacked wasi-sdk and it builds for wasm32-wasip1 and runs under node
# instead, which is how it is tested in the agent sandbox:
#
#   curl -sL -o /tmp/w.tar.gz https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/wasi-sdk-25.0-x86_64-linux.tar.gz
#   tar xzf /tmp/w.tar.gz -C /tmp
#   WASI_SDK=/tmp/wasi-sdk-25.0-x86_64-linux test/run.sh
set -e
here=$(dirname "$0")
out="${TMPDIR:-/tmp}/kb_protocol_test"
warn="-std=c11 -Wall -Wextra -Werror"
src="$here/kb_protocol_test.c $here/../kb_protocol.c"

if [ -n "$WASI_SDK" ]; then
    # shellcheck disable=SC2086
    "$WASI_SDK/bin/clang" --target=wasm32-wasip1 $warn -o "$out.wasm" $src
    cat > "$out.mjs" <<'JS'
import { WASI } from 'node:wasi';
import { readFile } from 'node:fs/promises';
const wasi = new WASI({ version: 'preview1', args: ['kb_protocol_test'], env: {} });
const mod = await WebAssembly.compile(await readFile(process.argv[2]));
process.exitCode = wasi.start(await WebAssembly.instantiate(mod, wasi.getImportObject()));
JS
    exec node --no-warnings "$out.mjs" "$out.wasm"
fi

# shellcheck disable=SC2086
"${CC:-cc}" $warn -o "$out" $src
exec "$out"
