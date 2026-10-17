#!/bin/bash
export TMPDIR="${TMPDIR:-/tmp}"
TMPDIR="${TMPDIR%/}"
# mdblist ratings[] (#205): GET /imdb/movie/tt... devolve source/value/score; o parser
# antigo (POST + "rating") deixava 0/N no aparelho.
set -eu
cd "$(dirname "$0")/.."
cc -O1 -g -Wall -Wextra -DNV_EXTRAS_TESTE -Isrc -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  src/extras.c src/js.c tests/notas1.c \
  -lpthread -o ${TMPDIR:-/tmp}/nuvio-notas1-tests
${TMPDIR:-/tmp}/nuvio-notas1-tests
