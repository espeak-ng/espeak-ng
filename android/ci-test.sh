#!/bin/bash
set -e

# Build and install the app (must use the same debug key as connectedAndroidTest)
cd android && ./gradlew assembleDebug --no-daemon && cd ..
adb install -r android/build/outputs/apk/debug/espeak-debug.apk

# Extract voice data — the activity finishes when extraction is done
# -W waits for the activity to call finish() before returning
adb shell am start -W -a android.speech.tts.engine.INSTALL_TTS_DATA -n com.reecedunn.espeak/.DownloadVoiceData
echo "Voice data extraction complete"

# Set eSpeak as the default TTS engine
adb shell settings put secure tts_default_synth com.reecedunn.espeak

# Debug: check voice data on device before running tests
echo "=== Voice data check ==="
adb shell "ls -la /data/user_de/0/com.reecedunn.espeak/app_voices/espeak-ng-data/ 2>/dev/null || echo 'device-protected: not found'"
adb shell "ls -la /data/user/0/com.reecedunn.espeak/app_voices/espeak-ng-data/ 2>/dev/null || echo 'regular: not found'"
echo "========================"

# Run instrumented tests
cd android && ./gradlew connectedAndroidTest --no-daemon --stacktrace
