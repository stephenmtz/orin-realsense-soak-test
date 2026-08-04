#!/usr/bin/env bash

set -euo pipefail

FIRMWARE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PRESET="${1:-Debug}"
IMAGE_TAG="stm32-flash-toolchain"

docker build -t "$IMAGE_TAG" "$FIRMWARE_DIR"

docker run --rm -it \
    --privileged \
    -v /dev/bus/usb:/dev/bus/usb \
    -v "$FIRMWARE_DIR":/workspace \
    -w /workspace/nucleo-f446re \
    "$IMAGE_TAG" \
    bash -c "cmake --preset ${PRESET} && cmake --build --preset ${PRESET} && cmake --build build/${PRESET} --target flash"
