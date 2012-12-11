/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.googlecode.eyesfree.espeak;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.Engine;
import android.util.Log;

import com.googlecode.eyesfree.espeak.SpeechSynthesis.SynthReadyCallback;
import com.googlecode.eyesfree.espeak.SpeechSynthesis.Voice;

import java.io.File;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;

public class CheckVoiceData extends Activity {
    private static final String TAG = "eSpeakTTS";

    private static final int REQUEST_DOWNLOAD = 1;

    /** Resources required for eSpeak to run correctly. */
    private static final String[] BASE_RESOURCES = {
            "intonations", "phondata", "phonindex", "phontab", "en_dict", "voices/en/en-us"
    };

    public static File getDataPath(Context context) {
        return new File(context.getDir("voices", MODE_WORLD_READABLE), "espeak-data");
    }

    public static boolean hasBaseResources(Context context) {
        final File dataPath = getDataPath(context);

        for (String resource : BASE_RESOURCES) {
            final File resourceFile = new File(dataPath, resource);

            if (!resourceFile.exists()) {
                Log.e(TAG, "Missing base resource: " + resourceFile.getPath());
                return false;
            }
        }

        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        checkForVoices(false);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_DOWNLOAD:
                checkForVoices(true);
                break;
        }
    }

    private void checkForVoices(boolean attemptedInstall) {
        final File dataPath = getDataPath(this);

        ArrayList<String> availableLanguages = new ArrayList<String>();
        ArrayList<String> unavailableLanguages = new ArrayList<String>();

        if (!hasBaseResources(this)) {
            if (!attemptedInstall) {
                downloadVoiceData();
                return;
            }
            // No base resource, can't load available voices.
            unavailableLanguages.add(Locale.ENGLISH.toString());
            returnResults(Engine.CHECK_VOICE_DATA_MISSING_DATA, dataPath, availableLanguages,
                    unavailableLanguages);
            return;
        }

        final SpeechSynthesis engine = new SpeechSynthesis(this, mSynthReadyCallback);
        final List<Voice> voices = engine.getAvailableVoices();

        for (Voice voice : voices) {
            availableLanguages.add(voice.toString());
        }

        final ArrayList<String> checkFor = getIntent().getStringArrayListExtra(
                TextToSpeech.Engine.EXTRA_CHECK_VOICE_DATA_FOR);

        if (checkFor != null && !checkFor.isEmpty()) {
            final Set<String> checkForSet = new HashSet<String>(checkFor);

            availableLanguages = filter(availableLanguages, checkForSet);
            unavailableLanguages = filter(unavailableLanguages, checkForSet);
        }

        returnResults(Engine.CHECK_VOICE_DATA_PASS, dataPath, availableLanguages,
                unavailableLanguages);
    }

    /**
     * Launches the voice data installer.
     */
    private void downloadVoiceData() {
        final Intent checkIntent = new Intent(this, DownloadVoiceData.class);

        startActivityForResult(checkIntent, REQUEST_DOWNLOAD);
    }

    private void returnResults(int result, File dataPath, ArrayList<String> availableLanguages,
            ArrayList<String> unavailableLanguages) {
        final Intent returnData = new Intent();
        returnData.putStringArrayListExtra(Engine.EXTRA_AVAILABLE_VOICES, availableLanguages);
        returnData.putStringArrayListExtra(Engine.EXTRA_UNAVAILABLE_VOICES, unavailableLanguages);

        // Don't bother returning Engine.EXTRA_VOICE_DATA_FILES,
        // Engine.EXTRA_VOICE_DATA_FILES_INFO, or
        // Engine.EXTRA_VOICE_DATA_ROOT_DIRECTORY
        // because they're don't seem necessary.

        setResult(result, returnData);
        finish();
    }

    /**
     * Filters a given array list, maintaining only elements that are in the
     * constraint. Returns a new list containing only the filtered elements.
     */
    private ArrayList<String> filter(ArrayList<String> in, Set<String> constraint) {
        final ArrayList<String> out = new ArrayList<String>(constraint.size());

        for (String s : in) {
            if (constraint.contains(s)) {
                out.add(s);
            }
        }

        return out;
    }

    private final SynthReadyCallback mSynthReadyCallback = new SynthReadyCallback() {
        @Override
        public void onSynthDataReady(byte[] audioData) {
            // Do nothing.
        }

        @Override
        public void onSynthDataComplete() {
            // Do nothing.
        }
    };
}
