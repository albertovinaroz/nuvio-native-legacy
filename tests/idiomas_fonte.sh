#!/bin/bash
# Idiomas de audio e de legenda da fonte (bandeiras da folha).
set -euo pipefail
cd "$(dirname "$0")/.."
DIR=$(mktemp -d /tmp/nuvio-idiomas-fonte.XXXXXX)
trap 'rm -rf "$DIR"' EXIT
flags=(-ffunction-sections -fdata-sections)
if [ "$(uname -s)" = Darwin ]; then flags+=(-Wl,-dead_strip); else flags+=(-Wl,--gc-sections); fi
cc src/stream_parse.c src/js.c src/badges.c tests/idiomas_fonte.c -Isrc \
  -I/opt/homebrew/include -O1 -g -Wall -Wextra -Wno-macro-redefined \
  "${flags[@]}" -o "$DIR/test"
"$DIR/test"
