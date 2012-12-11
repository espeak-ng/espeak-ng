# Android Port of eSpeak Text-to-Speech

This branch contains the files needed to build espeak on the android platform.
It is derived from the eyes-free project.

## Prerequisits

To build eSpeak for Android, you need to have the following installed and
configured correctly:

1.  Android SDK
2.  Android NDK
3.  Eclipse
4.  Android Developer Tools (ADT) for Eclipse

## Building

1.  Build the `libttsespeak.so` file by running:

        $ ndk-build
2.  Open Eclipse.
3.  Create a new workspace.
4.  Import the espeak folder as an exising Android project.
5.  Build the espeak apk within Eclipse.

## Installing

The generated `eSpeakActivity.apk` can be installed like any other apk build
via eclipse, such as by using the `Run` menu option.

After running, `eSpeakActivity` will download the `espeak-data.zip` file. To
enable eSpeak, you need to:

1.  go into the Android `Text-to-Speech settings` UI;
2.  enable `eSpeak TTS` in the `Engines` section;
3.  select `eSpeak TTS` as the default engine;
4.  use the `Listen to an example` option to check if everything is working.
