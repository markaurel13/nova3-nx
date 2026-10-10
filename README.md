<div align="center">

<img src="launcher/icon.jpg" alt="N.O.V.A. 3" width="160">

# nova3-nx

**N.O.V.A. 3: Near Orbit Vanguard Alliance for Nintendo Switch**

An unofficial native Nintendo Switch port / wrapper for the 32-bit Android release of  
**N.O.V.A. 3: Near Orbit Vanguard Alliance (v1.0.7)**.

[![Nintendo Switch](https://img.shields.io/badge/Nintendo_Switch-Homebrew-E60012?style=for-the-badge&logo=nintendoswitch&logoColor=white)](#)
[![Status](https://img.shields.io/badge/Status-Playable-brightgreen?style=for-the-badge)](#)
[![Architecture](https://img.shields.io/badge/AArch32-32--bit_Native-6A1B9A?style=for-the-badge)](#)
[![Graphics](https://img.shields.io/badge/Graphics-OpenGL_ES_2.0-FF6F00?style=for-the-badge&logo=opengl&logoColor=white)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](LICENSE)

</div>

---

A native Nintendo Switch port / wrapper of Gameloft's sci-fi FPS classic **N.O.V.A. 3: Near Orbit Vanguard Alliance** (Android version **1.0.7**, versionCode `1070`), built utilizing the **android32** execution runtime on Horizon OS.

This port runs the original ARMv7-A NEON native library (`libNOVA3_neon.so`) directly on the Switch hardware with full physical controller support, hardware-accelerated OpenGL ES 2.0 rendering via Mesa Nouveau, multi-threaded audio via OpenSL ES, and an optimized I/O caching subsystem.

---

## Features

- **Native Dual-Stick Controls**: Fully mapped for Nintendo Switch Joy-Cons and Pro Controllers.
- **Sprint on L3**: Left Stick click toggles sprint seamlessly according to modern FPS standards (double-stick flick sprint completely disabled for consistency).
- **Weapon Switch on L1**: Quick weapon switching mapped directly to L1 (shoulder button).
- **Skill on Y**: Special ability activation dedicated to the Y face button.
- **Hold-to-Aim ADS**: True hold-to-aim support on ZL (Left Trigger).
- **Mesa Nouveau OpenGL ES 2.0**: Native hardware acceleration via Tegra X1 GPU.
- **Low-Latency Audio**: Multi-buffered audio streaming backed by Horizon `audout` through OpenSL ES (zero underruns).
- **GLA File Cache & Async Save**: In-memory handle pooling for `.gla` archives and background thread saves to eliminate MicroSD I/O lag spikes.
- **Standalone NRO Launcher**: Includes a dedicated Homebrew Menu forwarder launcher with loading screen progress bar.

---

## Controls

| Nintendo Switch Button | Action |
|:---|:---|
| **Left Stick** | Moverse / Navigation |
| **Right Stick** | Mirar / Apuntar cámara (Camera Aim) |
| **L3 (Click Stick Izquierdo)** | **Correr / Sprint** (Toggle on/off) |
| **L1 (L)** | **Cambiar de Arma** (Next Weapon) |
| **Y** | **Habilidad Especial** (Special Ability) |
| **ZL** | **Apuntar con la mira (ADS)** (Hold to Aim) |
| **ZR** | **Disparar** (Primary Fire / Hip Fire) |
| **R (R1) / B** | **Lanzar Granada** (Grenade) |
| **X** | **Recargar** (Reload) |
| **A** | **Saltar / Confirmar** (Jump / Confirm) |
| **Plus (+)** | Menú de Pausa (Pause Menu) |
| **Minus (-)** | Cambiar de Arma alternativo / Objetivos |
| **D-Pad** | Movimiento direccional |
| **Pantalla Táctil** | Interacción nativa con la interfaz táctil |

---

## Requirements

To run this port, you need game assets from an official copy of **N.O.V.A. 3** for Android:
- Target version: **v1.0.7** (versionCode `1070` / `com.gameloft.android.ANMP.GloftN3HM`).
- The game's native library: `libNOVA3_neon.so`.
- The game's OBB data archives:
  - `main.1050.com.gameloft.android.ANMP.GloftN3HM.obb` (Base assets)
  - `patch.1070.com.gameloft.android.ANMP.GloftN3HM.obb` (Update patch assets)

---

## Installation Guide

### Step 1: Extract Game Assets (OBB 1050 + 1070)

The OBB files are standard ZIP archives containing the game data.

1. Open/extract `main.1050.com.gameloft.android.ANMP.GloftN3HM.obb` using 7-Zip, WinRAR, or your system unzip tool.
2. Open/extract `patch.1070.com.gameloft.android.ANMP.GloftN3HM.obb`.
3. **Important (Overwrite)**: Merge the extracted contents of **1070** into the extracted folder of **1050**, choosing **Overwrite all** so the updated 1070 patch files replace the older 1050 files.
4. Locate the resulting `files/` folder (which contains archives like `weapons_stream.gla`, `actors_stream.gla`, `.bdae`, `.etc`, etc.).

### Step 2: Extract the Native Library

From your `nova3.apk` (v1.0.7):
1. Rename `nova3.apk` to `nova3.zip` or open it with 7-Zip.
2. Extract `lib/armeabi-v7a/libNOVA3_neon.so`.

### Step 3: Copy Files to MicroSD

Place the files onto your Nintendo Switch MicroSD card following this exact directory layout:

```
sdmc:/switch/nova3/
├── nova3.nro
├── libNOVA3_neon.so
└── data/
    └── files/
        ├── sounds_hi_p1.gla
        ├── sounds_hi_p2.gla
        ├── sounds_hi_p3.gla
        ├── weapons_stream.gla
        ├── actors_stream.gla
        ├── ... (all extracted OBB assets)
```

> **Note**: Also keep a copy of `nova3.apk` in `sdmc:/switch/nova3/nova3.apk` if prompted.

### Step 4: Launching the Game

Launch **`nova3.nro`** from the **Homebrew Menu** via **Title Redirection** (hold R while launching any installed Switch game) to grant full RAM access. Do not use Applet Mode (Album).

---

## Recommended Overclock Settings (sys-clk)

N.O.V.A. 3 runs very well on Nintendo Switch, but the Tegra X1 dynamic governor may aggressively downclock or put the GPU into sleep mode during frames with heavy CPU translation spikes, causing artificial frame stutters.

To ensure consistent 60 FPS performance without draining battery, we recommend setting a modest, stable clock profile via **sys-clk**:

```ini
[0100777777777000]
handheld_cpu=1224
handheld_gpu=230
docked_cpu=1224
docked_gpu=230
```

### Why these frequencies?
- **CPU @ 1224 MHz**: A slight bump over stock (1020 MHz) provides plenty of breathing room for the 32-bit bionic translation layer and streaming decompression without generating excess heat.
- **GPU @ 230 MHz**: Keeps the GPU awake at a fixed, cool, battery-friendly baseline. Because the Switch's stock handheld GPU is 307–384 MHz and docked is up to 768 MHz, 230 MHz is actually *below or around stock*, but prevents the power governor from sleeping the GPU down to 76 MHz during CPU-bound asset streaming.

---

## Building from Source

### Prerequisites

- **Docker**
- Linux environment (or WSL2 on Windows)
- `git`

### Build Steps

1. Clone this repository alongside `libnx32`:

```bash
git clone https://github.com/aks796/libnx32.git
git clone https://github.com/<your-username>/nova3-switch.git
```

2. Build `libnx32` prefix:

```bash
cd libnx32
./build.sh
cd ..
```

3. Download prebuilt Mesa 32-bit (`mesa32`) into `portlibs32/`:

```bash
cd nova3-switch
mkdir -p portlibs32
wget https://github.com/aks796/mesa32/releases/download/mesa-20.1.0-rc3/mesa32.zip -O /tmp/mesa32.zip
unzip -q /tmp/mesa32.zip -d portlibs32/
```

4. Build the core 32-bit ExeFS payload (`nova3_nx.nsp`):

```bash
./build.sh
```

5. Build the Homebrew Menu launcher (`nova3.nro`):

```bash
cp nova3_nx.nsp launcher/
cd launcher
./build.sh
```

The compiled binaries `nova3.nro` and `nova3_nx.nsp` will be generated inside the `launcher/` directory.

---

## Project Structure

```
nova3-switch/
├── build.sh             # Main build entrypoint (runs Docker toolchain)
├── Makefile             # Port Makefile
├── portlibs32/          # Mesa 32-bit EGL/GLES2 libraries & headers
├── runtime/             # android32 Horizon runtime layer (bionic, JNI, audio, EGL)
├── launcher/            # Homebrew Menu NRO launcher with progress UI
├── source/              # Port-specific hooks, JNI bindings, and input wrappers
│   ├── port.c           # Main application loop, input loop, timing profiler
│   ├── port_jni.c       # JNI method implementations for Gameloft engine
│   ├── patch_game.c     # Native memory patches, engine crash fixes, hooks
│   ├── egl_helper.c     # EGL context initialisation & config helper
│   ├── imports.c        # Dynamic symbol resolution and shims
│   └── nx_mutex_fix.c   # Horizon POSIX threading fixups
└── tools/               # Auxiliary scripts and symbol definitions
```

---

## Credits & Acknowledgements

- **Gameloft**: Original developers of *N.O.V.A. 3: Near Orbit Vanguard Alliance*.
- **aks796**: For the [android32](https://github.com/aks796/android32) runtime and [libnx32](https://github.com/aks796/libnx32) toolchain.
- **Rinnegatamante**: For the reference work on PS Vita Android ports and kubridge ecosystem.
- **devkitPro**: For the devkitARM toolchain.
- **libnx**: For the Nintendo Switch homebrew library.

---

## Disclaimer

This repository does **not** contain any copyrighted game assets, proprietary code, or game data files from Gameloft. You must provide your own legally purchased copy of *N.O.V.A. 3* to play the game on Nintendo Switch.
