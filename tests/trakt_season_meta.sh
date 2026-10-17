#!/bin/bash
# Issue #512: addon season/episode for next episode + Trakt scrobble ids.
set -euo pipefail
cd "$(dirname "$0")/.."
dir=$(mktemp -d "${TMPDIR:-/tmp}/nuvio-trakt512.XXXXXX")
trap 'rm -rf "$dir"' EXIT
cc src/trakt.c src/metaep.c tests/stub_fichameta.c src/js.c src/metaprov.c src/traktscrobble.c tests/trakt_season_meta.c \
  -Isrc -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -o "$dir/teste" -O1 -g -Wall -Wextra -Wl,-dead_strip -lpthread
"$dir/teste" | grep -E 'trakt_season_meta|PASS'
