<div align="center">

<img src="launcher/icon.jpg" alt="N.O.V.A. 3" width="160">

# nova3-nx

**N.O.V.A. 3: Near Orbit Vanguard Alliance for Nintendo Switch**

An unofficial Nintendo Switch native wrapper for the 32-bit Android release of  
**N.O.V.A. 3: Near Orbit Vanguard Alliance**.

[![Nintendo Switch](https://img.shields.io/badge/Nintendo_Switch-Homebrew-E60012?style=for-the-badge&logo=nintendoswitch&logoColor=white)](#)
[![Status](https://img.shields.io/badge/Status-Playable-brightgreen?style=for-the-badge)](#)
[![Architecture](https://img.shields.io/badge/AArch32-32--bit_Native-6A1B9A?style=for-the-badge)](#)
[![Graphics](https://img.shields.io/badge/Graphics-OpenGL_ES_2.0-FF6F00?style=for-the-badge&logo=opengl&logoColor=white)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](LICENSE)

</div>

---

A native Nintendo Switch port / wrapper of Gameloft's sci-fi FPS classic **N.O.V.A. 3: Near Orbit Vanguard Alliance** (Android version 1.0.8e), built utilizing the **android32** execution runtime on Horizon OS.

This port runs the original ARMv7-A NEON native library (`libNOVA3_neon.so`) directly on the Switch hardware with full physical controller support, hardware-accelerated OpenGL ES 2.0 rendering via Mesa Nouveau, multi-threaded audio via OpenSL ES, and an optimized I/O caching subsystem.

---

## Features

- **Native Controller Support**: Full physical mapping for Nintendo Switch Joy-Cons and Pro Controllers.
- **Enhanced Camera Aiming**: Custom analog stick look with dynamic acceleration curves and inverted Y-axis support.
- **Sprint Macro**: Native L3 (Left Stick click) macro for instantaneous sprinting.
- **ADS Handling**: Hold-to-aim support on ZL (Left Trigger).
- **Mesa Nouveau OpenGL ES 2.0**: Native hardware acceleration via Tegra X1 GPU.
- **Low-Latency Audio**: Multi-buffered audio streaming backed by Horizon `audout` through OpenSL ES.
- **GLA File Cache**: In-memory handle pooling for Gameloft `.gla` audio and resource archives, eliminating microSD I/O lag spikes.
- **Standalone NRO Launcher**: Includes a dedicated Homebrew Menu forwarder launcher with loading screen progress bar.

---

## Controls

| Switch Button | In-Game Action |
|:---|:---|
| **Left Stick** | Move (Character navigation) |
| **Right Stick** | Look / Aim (Dual-stick camera) |
| **Left Stick Click (L3)** | **Sprint Macro** (Auto sprint forward) |
| **Right Stick Click (R3)** | Switch Weapon |
| **ZR** | Shoot / Primary Fire |
| **ZL** | Aim Down Sights (ADS) |
| **R** | Shoot / Secondary Fire |
| **L** | Reload / Interact |
| **A** | Confirm / Action |
| **B** | Jump |
| **X** | Special Ability / Action |
| **Y** | Reload |
| **Plus (+)** | Pause Menu |
| **Minus (-)** | Objective / Select |
| **Touchscreen** | Native Multi-Touch UI interaction |

---

## Requirements

To run this port, you must legally own a copy of *N.O.V.A. 3: Near Orbit Vanguard Alliance* for Android (tested on **v1.0.8e**).

You will need the following files extracted from your official game installation:
1. `libNOVA3_neon.so` (located inside the APK under `lib/armeabi-v7a/libNOVA3_neon.so`).
2. The game assets folder (`data/` or extracted contents from the OBB archive).

---

## Installation

1. Download the latest release from the [Releases](https://github.com/) section.
2. Extract the archive onto your Nintendo Switch microSD card.
3. Place `nova3.nro` into `sdmc:/switch/nova3/`.
4. Copy `libNOVA3_neon.so` to `sdmc:/switch/nova3/libNOVA3_neon.so`.
5. Copy your game asset files to `sdmc:/switch/nova3/data/files/`.

Your microSD card directory structure should look like this:

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
        └── ...
```

6. Launch the game from the **Homebrew Menu** (Title Redirection / full RAM access recommended).

---

## Building from Source

### Prerequisites

- **Docker** (recommended)
- Linux environment (or WSL2 on Windows)
- `git`

### Build Steps

1. Clone this repository alongside `libnx32`:

```bash
git clone https://github.com/aks796/libnx32.git
git clone https://github.com/<your-username>/nova3-switch.git
```

2. Build `libnx32` prefix (if not already present):

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

The resulting `nova3.nro` and `nova3_nx.nsp` will be generated inside the `launcher/` directory.

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
