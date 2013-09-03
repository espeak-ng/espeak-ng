# Android Port of eSpeak Text-to-Speech

This branch contains the files needed to build espeak on the android platform.
It is derived from the eyes-free project.

## Prerequisits

To build eSpeak for Android, you need to have the following installed and
configured correctly:

1.  Android SDK
2.  Android NDK
3.  wxWidgets 2.8 (for espeakedit when building `android/res/raw/espeakdata.zip`)

If you are building with Eclipse, you will also need:

1.  Eclipse
2.  Android Developer Tools (ADT) for Eclipse

If you are building on the command line, you will also need either:

1.  ant (e.g. run `sudo apt-get install ant` on a Debian-based distribution), or
2.  gradle 1.7, which can be installed from the
    [Ubuntu PPA](https://launchpad.net/~cwchien/+archive/gradle/+files/gradle-1.7_1.0-0ubuntu1_all.deb)
    debian file (including on Debian systems)

## Building with Gradle

1.  Set the location of the Android SDK:

2.  Build the project:

        $ ./autogen.sh
        $ ./configure
        $ make

This will create an `android/build/apk/espeak-release-unsigned.apk` file.

## Building with Eclipse

1.  Build the JNI binding and espeak data file by running:

        $ ./autogen.sh
        $ ./configure
        $ make jni espeakdata
2.  Open Eclipse.
3.  Create a new workspace.
4.  Import the espeak folder as an exising Android project.
5.  Build the espeak apk within Eclipse.

The generated `eSpeakActivity.apk` can be installed like any other apk build
via eclipse, such as by using the `Run` menu option.

## Building with Ant

1.  Build the JNI binding and espeak data file by running:

        $ ./autogen.sh
        $ ./configure
        $ make jni espeakdata
2.  Update the project using the Android utility which is part of the SDK:

        $ cd android
        $ android update project -s -t 1 -p .
3.  Build the package.

        $ ant release

## Signing the APK

In order to install the built APK (e.g. `bin/eSpeakActivity-release-unsigned.apk`)
you need to self-sign the package. You can do this by:

1.  Creating a certificate, if you do not already have one:

        $ keytool -genkey -keystore [YOUR_CERTIFICATE] -alias [ALIAS]
2. Sign the package using your certificate:

        $ jarsigner -sigalg MD5withRSA -digestalg SHA1 \
          -keystore [YOUR_CERTIFICATE] \
          bin/eSpeakActivity-release-unsigned.apk [ALIAS]
3. Align the apk using the zipalign tool.

        $ zipalign 4 bin/eSpeakActivity-release-unsigned.apk \
          bin/eSpeakActivity-release-signed.apk

## Installing the APK

Now, you can install the APK using the `adb` tool:

    $ adb install -r bin/eSpeakActivity-release-signed.apk

## Enabling eSpeak on the Device

After running, `eSpeakActivity` will extract the `espeakdata.zip` file into its
own data directory to set up the available voices.

To enable eSpeak, you need to:

1.  go into the Android `Text-to-Speech settings` UI;
2.  enable `eSpeak TTS` in the `Engines` section;
3.  select `eSpeak TTS` as the default engine;
4.  use the `Listen to an example` option to check if everything is working.
