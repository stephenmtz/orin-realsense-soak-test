FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi \
        libstdc++-arm-none-eabi-newlib \
        openocd \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
# (TODO) building on pi for portability but build on desktop -> upload image to google bucket 
# could create a workflow to build the file too... and then push to bucket but we shall see