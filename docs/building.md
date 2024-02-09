# Building eSpeak NG

- [CMake](#cmake)
  - [Dependencies](#dependencies)
  - [Build options](#build-options)
  - [Building](#building)
  - [Testing](#testing)
  - [Installing](#installing)
  - [Sanitizers](#sanitizers)
  - [LLVM Fuzzer Support](#llvm-fuzzer-support)
- [Android](#android)
- [Windows](#windows)

> [!NOTE]
> For legacy autotools-based build documentation, see [building-ac.md](building-ac.md)

## CMake

eSpeak NG using CMake build system.

### Dependencies

Additionally to CMake and toolchain (gcc/clang/msvc supported), you may need a following libraries:

- [`pcaudiolib`](https://github.com/espeak-ng/pcaudiolib) - cross-platform audio output support
- [`sonic`](https://github.com/espeak-ng/sonic) - sonic audio speed up support (> 450 wpm)
- `ronn` - man-pages building
- `kramdown` - documentation building

### Build options

Following configuration options may be passed to CMake to configure build process:

| Config name             | Type | Description                            | Default |
|-------------------------|------|----------------------------------------|---------|
| `BUILD_SHARED_LIBS`     | BOOL | Build shared espeak-ng.so library      | OFF     |
| `ESPEAK_BUILD_MANPAGES` | BOOL | Build manpages (requires `ronn`)       | ON      |
| `ESPEAK_COMPAT`         | BOOL | Install compat binary symlinks         | OFF     |
| `EXTRA_cmn`             | BOOL | Compile extra cmn dictionary           | ON      |
| `EXTRA_ru`              | BOOL | Compile extra ru dictionary            | ON      |
| `EXTRA_yue`             | BOOL | Compile extra yue dictionary           | ON      |
| `USE_MBROLA`            | BOOL | Use mbrola for speech synthesis        | ON      |
| `USE_LIBSONIC`          | BOOL | Use libsonic for faster speech rates   | ON      |
| `USE_LIBPCAUDIO`        | BOOL | Use libPcAudio for sound output        | ON      |
| `USE_KLATT`             | BOOL | Use klatt for speech synthesis         | ON      |
| `USE_SPEECHPLAYER`      | BOOL | Use speech-player for speech synthesis | ON      |
| `USE_ASYNC`             | BOOL | Support asynchronous speech synthesis  | ON      |

You can use following option to see all configurable options in CMake:

    cmake -LH -Bbuild

### Building

The first time you build eSpeak NG, or when you want to change how to build
eSpeak NG, you need to run the following standard cmake commands:

    cmake -B build

To select installation prefix, you may change `CMAKE_INSTALL_PREFIX` build option (`/usr/local` by default).

To use a different compiler, you may change environment variables, or use [CMake Toolchain file](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html).

The `espeak-ng` program, along with the espeak-ng voices, can then be built with:

    cmake --build build

### Testing

Before installing, you can test the built espeak-ng using the following command
from the top-level directory of this project:

    ctest --test-dir build -Ttest --output-on-failure

You can run tests in parallel to speed up. Use `-jN` (job count) option as for building.

### Installing

You can install eSpeak NG by running the [cmake --install build](https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project).

### Sanitizers

> [!NOTE]
> TODO: write sanitizers usage docs

### LLVM Fuzzer Support

> [!NOTE]
> TODO: write fuzzer usage docs

## Android

> [!NOTE]
> TODO: write Android build docs

## Windows

> [!NOTE]
> TODO: write Windows build docs
