#!/bin/bash
export TMPDIR="${TMPDIR:-/tmp}"
TMPDIR="${TMPDIR%/}"
# Compila e roda no Mac, com a mesma pasta de arte do pacote da TV.
#
# FICA LOGADO ENTRE EXECUCOES. A sessao mora em $NUVIO_DADOS, que aqui aponta
# para ~/.nuvio: logue UMA vez (escaneando o QR da tela) e todo arranque
# seguinte abre com a conta dentro, porque o refresh token se renova sozinho.
# CONFERIDO: 1o arranque "[sessao] logado como ..."; reiniciando sem escanear
# nada, "[sessao] sessao restaurada ..." e "[addons] N vindos da conta".
#
# Para testar como um usuario NOVO (a primeira execucao de quem instala),
# aponte a variavel para uma pasta vazia:
#     NUVIO_DADOS=/tmp/nuvio-novo bash tools/mac.sh
#
# "Sair da conta", em Ajustes, apaga ~/.nuvio inteiro (sessao, ajustes e
# progresso) — depois disso e preciso escanear de novo.
set -e
cd "$(dirname "$0")/.."
# Os -D do servidor saem do MESMO local.properties do app web (tools/env.sh).
# Sem eles o app compila, instala e abre — e o unico sintoma e a tela de login
# dizendo "Este pacote foi montado sem servidor". Nenhum desses valores entra em
# arquivo de codigo versionado.
ENV_D=$(tools/env.sh)
# Fora da pasta do pacote: o Mac nao deve gravar sessao dentro de deploy/.
export NUVIO_DADOS="${NUVIO_DADOS:-$HOME/.nuvio}"
# NUVIO_MAC_BIN: outro binario (a auditoria nao pisa no /tmp/nuvio-native-legacy-mac
# de quem estiver rodando o app). NUVIO_MAC_SO_COMPILAR=1: compila e sai.
BIN="${NUVIO_MAC_BIN:-${TMPDIR:-/tmp}/nuvio-native-legacy-mac}"
# Nucleo EBML/NAL do mkv.c em Rust, ligado por padrao (docs/plans/rust-mapa.md);
# NUVIO_RUST_MKVCORE=0 volta ao caminho C.
RUST_D=()
# Fork: sem cargo no Mac, o caminho C em vez de parar o build (avisa uma vez).
if [ "${NUVIO_RUST_MKVCORE:-1}" = 1 ] && ! command -v cargo >/dev/null 2>&1; then
  echo "mac.sh: cargo nao encontrado; compilando o mkv.c em C (NUVIO_RUST_MKVCORE=0)" >&2
  NUVIO_RUST_MKVCORE=0
fi
[ "${NUVIO_RUST_MKVCORE:-1}" = 1 ] && RUST_D=(-DNV_RUST_MKVCORE "$(tools/rust-mkvcore.sh)")
# NV_GIF_PREVIA (fork): o GIF anima como no .tpk da Samsung (avatar, capa de colecao).
eval cc src/*.c src/dts/*.c -o "$BIN" -O1 -g -DNV_GIF_PREVIA "$ENV_D" ${RUST_D[@]+'"${RUST_D[@]}"'} \
  -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz \
  -framework OpenGL -Wno-deprecated-declarations
[ -n "$NUVIO_MAC_SO_COMPILAR" ] && exit 0
exec "$BIN" "$(pwd)/deploy/app/art" "$@"
