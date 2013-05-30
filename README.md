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

If you are building on the command line, you will also need:

1.  ant (e.g. run `sudo apt-get install ant` on a Debian-based distribution)

## Building eSpeak

The eSpeak language data file and the JNI bindings needed for the Android
APK can be build using the following commands:

    $ ./autogen.sh
    $ ./configure --prefix=/usr
    $ make android

## Building the APK with Eclipse

1.  Open Eclipse.
2.  Create a new workspace.
3.  Import the espeak folder as an exising Android project.
4.  Build the espeak apk within Eclipse.

The generated `eSpeakActivity.apk` can be installed like any other apk build
via eclipse, such as by using the `Run` menu option.

## Building the APK from the Command Line

1.  Update the project using the Android utility which is part of the SDK:

        $ cd android
        $ android update project -s -t 1 -p .
2.  Build the package.

        $ ant release

In order to install the built `bin/eSpeakActivity-release-unsigned.apk` APK,
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
