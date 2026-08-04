FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi \
        openocd \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
