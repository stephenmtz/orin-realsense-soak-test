# Firmware

FreeRTOS + STM32 HAL firmware for the Nucleo-F446RE board that runs the soak-test rig's
sensing: it reads an HTS221 temperature/humidity sensor over I2C and streams readings out
over UART. Heater control via a PWM-driven MOSFET is in progress (see [Status](#status)).

See the [top-level README](../README.md) for the full rig overview and step-by-step usage.

## Layout

- `nucleo-f446re/` — CMake project, HAL config, startup code, `Src/main.cpp`
  (STM32CubeIDE / VS Code extension project)
- `drivers/` — low-level peripheral drivers (`hts221.*`, `mosfet.*`)
- `driver_tasks/` — FreeRTOS tasks wrapping each driver (`hts221_task.*`,
  `heater_task.*`)
- `third_party/` — vendored CMSIS, FreeRTOS, and STM32F4xx HAL sources
- `Dockerfile` / `flash.sh` — containerized ARM toolchain for building and flashing
  without a local install

## Build & flash

### Docker (recommended — no local toolchain needed)

```bash
./flash.sh [Debug|Release]
```

Builds a toolchain image (`gcc-arm-none-eabi`, `cmake`, `ninja`, `openocd`), configures
and builds with the given CMake preset, and flashes over ST-Link. Requires the board
connected over USB and Docker able to access `/dev/bus/usb` (the script runs the
container `--privileged`).

### Manual

Requires `arm-none-eabi-gcc`, `cmake`, `ninja`, and `openocd` installed locally.

```bash
cd nucleo-f446re
cmake --preset Debug
cmake --build --preset Debug
cmake --build build/Debug --target flash
```

## Hardware pin map (Nucleo-F446RE)

| Peripheral | Pins            | Use                                    |
| ---------- | ---------------- | --------------------------------------- |
| I2C1       | PB8 (SCL), PB9 (SDA) | HTS221 temperature/humidity sensor  |
| USART2     | PA2 (TX), PA3 (RX)   | 115200 8N1 console, also read by the telemetry bridge |

## Firmware behavior

- On boot, brings up the UART2 console and I2C1, then starts the HTS221 polling task
  (1 Hz).
- `ConsoleTask` prints `temp=<C> humidity=<%RH>` once a second over UART — this is the
  exact line format `telemetry/uart_to_influx.py` parses, so keep them in sync if you
  change it.
- Heater control (`drivers/mosfet.*`, `driver_tasks/heater_task.*`) is scaffolding only
  and is not yet called from `main.cpp`.

## Status

- Working: HTS221 driver + FreeRTOS task, UART console, CI checks.
- In progress: `drivers/mosfet.cpp` is unfinished (`init()` is not implemented) and
  `driver_tasks/heater_task.*` are empty stubs. Neither is wired into `main.cpp` yet.

## CI

`.github/workflows/firmware-checks.yml` runs on every push/PR touching `firmware/**`:

1. `clang-format --dry-run --Werror` over `drivers/`, `driver_tasks/`, and `main.cpp`
2. `cmake --preset Debug` + build
3. `clang-tidy` over `drivers/` and `driver_tasks/` (non-blocking)
