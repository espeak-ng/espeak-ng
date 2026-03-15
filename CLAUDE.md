# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

eSpeak NG is an open-source formant-based speech synthesizer supporting 100+ languages. It's a C project built with CMake, producing both the `espeak-ng` CLI and `libespeak-ng` library.

## Build Commands

```bash
# Configure (from repo root)
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug

# Build the binary and library
cmake --build build

# Build language/phoneme data (required before running or testing)
cmake --build build --target data

# Build compiled test binaries
cmake --build build --target tests

# Run all tests
ctest --test-dir build -j$(nproc) --output-on-failure

# Run a specific test by name
ctest --test-dir build -R language-pronunciation --output-on-failure

# Run the built binary
ESPEAK_DATA_PATH=$(pwd)/build LD_LIBRARY_PATH=$(pwd)/build/src:${LD_LIBRARY_PATH} \
  build/src/espeak-ng -v en "Hello world"

# Phoneme output (useful for debugging pronunciation)
ESPEAK_DATA_PATH=$(pwd)/build LD_LIBRARY_PATH=$(pwd)/build/src:${LD_LIBRARY_PATH} \
  build/src/espeak-ng -xq -v en "Hello world"
```

Key CMake options: `-DUSE_MBROLA=OFF`, `-DUSE_KLATT=ON`, `-DUSE_LIBSONIC=OFF`, `-DUSE_LIBPCAUDIO=OFF`.

## Architecture

### TTS Pipeline

```
Text → Encoding (encoding.c) → SSML parsing (ssml.c) → Clause parsing (readclause.c)
     → Dictionary lookup (dictionary.c) → Translation (translate.c, translateword.c)
     → Phoneme list (phonemelist.c) → Stress/length (setlengths.c)
     → Synthesis (synthesize.c) → Audio (wavegen.c)
```

Alternative synthesis backends: Klatt (`klatt.c`), MBROLA (`mbrowrap.c`, `synth_mbrola.c`).

### Key Source Directories

- **`src/libespeak-ng/`** — Core library: translation engine, synthesis, phoneme processing
- **`src/include/espeak-ng/`** — Public API headers (`espeak_ng.h`, `speak_lib.h`)
- **`dictsource/`** — Language pronunciation rules (`*_rules`) and exception lists (`*_list`)
- **`phsource/`** — Phoneme definitions (`ph_*`), intonation patterns, envelope data
- **`espeak-ng-data/lang/`** — Language configuration files (voice parameters, phoneme table selection)
- **`tests/`** — C tests (`test_*.c`) and shell-based tests (`*.test`)

### Language System

Each language involves three layers:
1. **Voice file** (`espeak-ng-data/lang/`): sets language parameters, phoneme table, dictionary name
2. **Dictionary rules** (`dictsource/<lang>_rules`, `<lang>_list`): letter-to-phoneme conversion rules and word exceptions
3. **Phoneme definitions** (`phsource/ph_<lang>`): defines phonemes used by the language, references base phoneme tables

Language-specific translation behavior is configured in `tr_languages.c` (sets up `Translator` struct with language options).

### Test Structure

Shell tests (`.test` files) use helper functions from `tests/common`:
- `test_phon <lang> <expected_phonemes> <text>` — verifies phoneme output for text
- `test_wav <voice> <expected_sha1> <text>` — verifies synthesized audio hash
- `test_replace <lang> <text> <expected>` — verifies dictionary translation trace

Tests run from the build directory with `ESPEAK_DATA_PATH=.` and `LD_LIBRARY_PATH=src:$LD_LIBRARY_PATH`.

## Common Workflows

### Adding/modifying pronunciation for a language

1. Edit rules in `dictsource/<lang>_rules` or exceptions in `dictsource/<lang>_list`
2. Rebuild data: `cmake --build build --target data`
3. Test: `ctest --test-dir build -R language-pronunciation --output-on-failure`
4. Verify manually: `ESPEAK_DATA_PATH=$(pwd)/build build/src/espeak-ng -xq -v <lang> "text"`

### Adding a test case

Add `test_phon` calls to the appropriate `.test` file in `tests/` (e.g., `language-pronunciation.test` for pronunciation, `language-phonemes.test` for phoneme output).

### Rebuilding after C source changes

Only `cmake --build build` is needed (no need to rebuild data unless `dictsource/` or `phsource/` changed).
