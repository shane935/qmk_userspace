#!/bin/sh
# Builds and runs the kb_protocol unit test natively. kb_protocol.c includes no
# QMK headers, which is the whole reason it is a separate file, so this needs
# nothing but a C compiler -- no QMK checkout, no toolchain, no keyboard.
set -e
here=$(dirname "$0")
out="${TMPDIR:-/tmp}/kb_protocol_test"
"${CC:-cc}" -std=c11 -Wall -Wextra -Werror -o "$out" "$here/kb_protocol_test.c" "$here/../kb_protocol.c"
"$out"
