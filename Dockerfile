# ---------- Stage 1: Build ----------
FROM alpine:3.20 AS build

RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    openssl-dev \
    zlib-dev \
    linux-headers

# --- Build & install DPP ---
WORKDIR /opt/build
RUN git clone --depth 1 https://github.com/brainboxdotcc/DPP.git dpp && \
    mkdir dpp/build && cd dpp/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DDPP_BUILD_TEST=OFF .. && \
    cmake --build . --parallel && \
    cmake --install .

# --- Build your project (mirrors build.sh) ---
WORKDIR /app
COPY . .
RUN rm -rf build && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build .

# ---------- Stage 2: Runtime ----------
FROM alpine:3.20 AS runtime

RUN apk add --no-cache \
    libstdc++ \
    libssl3 \
    libcrypto3 \
    zlib

COPY --from=build /usr/local/lib/libdpp.so* /usr/local/lib/
ENV LD_LIBRARY_PATH=/usr/local/lib

WORKDIR /app
COPY --from=build /app/build/cubic-verification /app/cubic-verification

RUN addgroup -S bot && adduser -S bot -G bot && chown -R bot:bot /app
USER bot

ENTRYPOINT ["/app/cubic-verification"]
