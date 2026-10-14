# Compilador do .tpk para o workflow do fork: o MESMO tools/tpk/Dockerfile
# (Debian buster armel, glibc 2.28), com o apt no snapshot.debian.org.
# MEDIDO em 14/10/2026: archive.debian.org nao serve mais NENHUM binario armel
# (404 em todo o pool armel; amd64/arm64/armhf seguem 200). O snapshot de
# 01/06/2023 ainda e o buster inteiro. Arquivo separado para nao conflitar com
# o upstream quando ele corrigir o dele.
FROM --platform=linux/arm/v5 arm32v5/debian:buster-slim
RUN printf 'deb [check-valid-until=no] http://snapshot.debian.org/archive/debian/20230601T000000Z buster main\n' > /etc/apt/sources.list && \
    apt-get -qq -o Acquire::Retries=5 update && apt-get -qq -o Acquire::Retries=5 install -y --no-install-recommends \
      gcc g++ make pkg-config file libgles2-mesa-dev libegl1-mesa-dev zlib1g-dev \
      ca-certificates && rm -rf /var/lib/apt/lists/*
ENV CFLAGS="-O2 -fPIC -march=armv7-a -mfloat-abi=softfp -mfpu=neon" \
    CXXFLAGS="-O2 -fPIC -march=armv7-a -mfloat-abi=softfp -mfpu=neon"
