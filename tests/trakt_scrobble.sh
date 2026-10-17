#!/bin/bash
export TMPDIR="${TMPDIR:-/tmp}"
TMPDIR="${TMPDIR%/}"
# Decisao e corpo do scrobble do Trakt (#179). Sem SDL e sem rede.
set -eu
cd "$(dirname "$0")/.."
cc src/traktscrobble.c src/metaep.c src/js.c tests/trakt_scrobble.c -Isrc -o ${TMPDIR:-/tmp}/nuvio-trakt-scrobble-tests -O1 -g -Wall
${TMPDIR:-/tmp}/nuvio-trakt-scrobble-tests
