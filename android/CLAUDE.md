# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with the Android app in this directory.

## Build Commands

```bash
# Build debug APK (from android/ directory)
./gradlew assembleDebug

# Build release APK (unsigned)
./gradlew assembleRelease

# Run instrumentation tests (requires connected device/emulator)
./gradlew connectedAndroidTest
```

Output APKs land in `build/outputs/apk/debug/` and `build/outputs/apk/release/`.

## Build Configuration

- compileSdk 34, minSdk 21, targetSdk 33
- NDK 29.0.14206865, CMake 3.22.1
- JDK 17 (Temurin) in CI

The Gradle build has custom tasks that run automatically:
1. CMake builds `libttsespeak.so` (JNI) + generates `espeak-ng-data/`
2. `createDataArchive` zips the data into `res/raw/espeakdata.zip`
3. `createDataHash` generates SHA256 for upgrade detection
4. `createDataVersion` writes the hash to `res/raw/espeakdata_version`

Native build disables `USE_ASYNC` and `USE_MBROLA` (not needed on Android).

## Architecture

```
Android TTS Framework
        │
   TtsService (TextToSpeechService)
        │
   SpeechSynthesis (Java JNI wrapper)
        │  JNI calls
   eSpeakService.c (jni/jni/) — bridge layer
        │  C API (espeak_*)
   libespeak-ng (../../src/libespeak-ng/)
```

### Key Classes (`src/com/reecedunn/espeak/`)

- **TtsService** — Android TTS engine service; handles `onSynthesizeText()`, voice selection, parameter setup
- **SpeechSynthesis** — JNI wrapper; loads `libttsespeak.so`, exposes native functions as Java API
- **VoiceSettings** — SharedPreferences wrapper for rate, pitch, volume, punctuation, variant
- **LanguageSettings** — Filters available voices by user-selected languages
- **CheckVoiceData** — Intent handler that verifies voice data files exist on device
- **DownloadVoiceData** — Extracts `espeakdata.zip` to device-protected storage
- **eSpeakActivity** — Demo/launcher activity for manual testing
- **TtsSettingsActivity** — Preferences UI (voice variant, rate, pitch, etc.)
- **Voice / VoiceVariant** — Data models for voice metadata and variant parsing

### JNI Layer (`jni/jni/eSpeakService.c`)

11 JNI functions mapping `SpeechSynthesis.native*()` Java methods to `espeak_*()` C API calls. Audio flows back via `SynthCallback` → `nativeSynthCallback()` → `SynthesisCallback.audioAvailable()`.

### Voice Data Lifecycle

On first launch (or version mismatch), `DownloadVoiceData` extracts `res/raw/espeakdata.zip` to device-protected storage. `CheckVoiceData` validates required files: `version`, `intonations`, `phondata`, `phonindex`, `phontab`, `en_dict`.

## Source Layout

```
android/
├── src/com/reecedunn/espeak/   # Java sources (18 classes)
│   └── preference/             # Custom preference widgets (5 classes)
├── jni/
│   ├── CMakeLists.txt          # Native build (links espeak-ng + JNI)
│   ├── jni/eSpeakService.c     # JNI bridge (344 lines)
│   └── include/                # config.h, Log.h
├── res/                        # Resources (40+ locale translations)
├── eSpeakTests/                # Instrumentation tests
├── build.gradle                # Gradle config
└── AndroidManifest.xml         # Services, activities, intents
```

## Testing

Tests are in `eSpeakTests/src/com/reecedunn/espeak/test/` — instrumentation tests covering voice enumeration, settings, variant parsing, data checking, and synthesis. They require a connected device or emulator.

## Common Workflows

### Modifying JNI bindings

1. Add/change native method declaration in `SpeechSynthesis.java`
2. Implement in `jni/jni/eSpeakService.c` (function name follows JNI convention: `Java_com_reecedunn_espeak_SpeechSynthesis_<methodName>`)
3. `./gradlew assembleDebug` rebuilds native libs automatically

### Updating voice data

Voice data comes from the parent project's `dictsource/` and `phsource/`. The Gradle build runs CMake which rebuilds `espeak-ng-data/`, then zips it. Just run `./gradlew assembleDebug`.

### Adding a new preference/setting

1. Add preference key constant in `VoiceSettings.java`
2. Add preference XML in `res/xml/` or programmatically in `TtsSettingsActivity.java`
3. Wire it through `TtsService.onSynthesizeText()` to the appropriate `SpeechSynthesis` parameter
