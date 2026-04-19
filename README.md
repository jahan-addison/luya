<h5 align="center">
  Luya 2D Gaming Engine 🎮
</h5>

<table align="center">
<tr>
<td>

```text
         _   _   _ __   __ _
        | | | | | |\ \ / // \
        | | | | | | \ V // _ \
        | |_| |_| |  | |/ ___ \
        |___|\___/   |_/_/   \_\

              2D Gaming Engine
                on Teensy 4.1
```
</td>
</tr>
</table>

## Overview

The Teensy 4.1 is a microcontroller development board based on the NXP i.MX RT1062, an ARM Cortex-M7 running at up to 600 MHz. With hardware floating-point, a dedicated SPI bus, and a built-in SDIO SD card slot, it provides a capable foundation for a 2D gaming engine driving an Adafruit ILI9341 TFT display.

<table border="0">
  <tr>
    <td width="220">
      <img src="/docs/teensy-front.jpg" width="200">
    </td>
    <td>
      Luya targets the i.MX RT1062 at 600 MHz with FPv5-D16 hard-float ABI. The display component wraps <code>ILI9341_t3</code> (or the DMA-capable <code>ILI9341_t3n</code>), audio is routed through the Teensy Audio Library via I2S to the SGTL5000 codec, and asset storage is served from the built-in SDIO SD card slot via <code>SdFat</code>.
    </td>
  </tr>
</table>

## Architecture

The engine is structured around three components:

| Component | Class | Library |
|-----------|-------|---------|
| Display | `luya::display::Display` |
| Audio | `luya::Audio` | Teensy Audio Library |
| Storage | `luya::Storage` | SdFat |

`Engine::init()` is called from Teensy `setup()` and `Engine::tick()` from `loop()`.

### Display drivers

The display component is polymorphic and defaults to `SDL2`, the drivers live in `luya/display/`:

| Driver | Class | Target |
|--------|-------|--------|
| Adafruit 2.8" TFT | `Adafruit_Display` | Teensy 4.1 hardware  |
| SDL2 | `SDL_Display` | Local development |

The SDL2 driver opens a desktop window at the ILI9341 native resolution (320×240) scaled up by `config::scale` (3×, 960×720). `SDL_RenderSetLogicalSize` ensures all draw calls use the same coordinate space as the Adafruit.

## Build

### macOS — SDL2

SDL2 is required for all host builds:

```bash
brew install sdl2
```

Then build:

```bash
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZER="Address;Undefined" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/luya
```

---

Run the test suite:

```bash
./build/test_suite
```

### Debian Based — SDL2

Install dependencies:

```bash
sudo apt update
sudo apt install cmake ninja-build libsdl2-dev
```

Then build:

```bash
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZER="Address;Undefined" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/luya
```

Run the test suite:

```bash
./build/test_suite
```

### Windows — MinGW + SDL2

Install [MSYS2](https://www.msys2.org/), then from an **MSYS2 MinGW64** shell install dependencies:

```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-SDL2
```

```bash
cmake -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/luya.exe
```

Run the test suite:

```bash
./build/test_suite.exe
```

> **Note:** The MSVC toolchain is not supported. Use the MinGW64 shell from MSYS2, not a Visual Studio developer prompt.

### Teensy 4.1 — cross-compile

Install [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) to get `arm-none-eabi-gcc` and the Teensy libraries, then:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-teensy41.cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To use the DMA-capable `ILI9341_t3n` display driver instead of `ILI9341_t3`, add `-DUSE_ILI9341_T3N=ON`.

If Teensyduino is not installed at the default Arduino.app path, override with:

```bash
-DTEENSYDUINO_PATH=/path/to/your/teensyduino/hardware/teensy
```

## Dependencies

Teensy libraries (provided by Teensyduino, resolved via `cmake/teensy.cmake`):

- `teensy4_core` — Teensy 4.1 hardware abstraction and startup
- `ILI9341_t3` / `ILI9341_t3n` — ILI9341 TFT display drivers
- `SdFat` — SD card filesystem via built-in SDIO
- `Teensy Audio Library` — I2S audio pipeline and SGTL5000 codec

Host-only dependencies

- `doctest` — Fast, header-only testing framework
- `fmt` — Fast, type-safe formatting library
- `matchit` — Pattern matching for C++
- `cmake-scripts` — Additional CMake utilities
- `SDL2` — Default display driver for local development

## Licensing

This project is dual-licensed under the **Apache License, Version 2.0** or the **GNU General Public License, Version 3.0 (or later)**.

You are free to choose the license that best fits your specific use case. For the full text of each license, please see [LICENSE.Apache-v2](LICENSE.Apache-v2) and [LICENSE-GPL-v3](LICENSE.GPL-v3).

