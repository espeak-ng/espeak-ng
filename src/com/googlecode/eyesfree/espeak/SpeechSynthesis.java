/*
 * Copyright (C) 2011 Google Inc.
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

import android.content.Context;
import android.speech.tts.TextToSpeech;
import android.util.Log;

import java.io.File;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;

public class SpeechSynthesis {
    private static final String TAG = SpeechSynthesis.class.getSimpleName();

    public static final int GENDER_MALE = 1;
    public static final int GENDER_FEMALE = 2;

    static {
        System.loadLibrary("ttsespeak");

        nativeClassInit();
    }

    private final Context mContext;
    private final SynthReadyCallback mCallback;
    private final String mDatapath;

    private boolean mInitialized = false;

    public SpeechSynthesis(Context context, SynthReadyCallback callback) {
        // First, ensure the data directory exists, otherwise init will crash.
        final File dataPath = CheckVoiceData.getDataPath(context);

        if (!dataPath.exists()) {
            Log.e(TAG, "Missing voice data");
            dataPath.mkdirs();
        }

        mContext = context;
        mCallback = callback;
        mDatapath = dataPath.getParentFile().getPath();

        attemptInit();
    }

    @Override
    protected void finalize() {
        nativeDestroy();
    }

    public int getSampleRate() {
        return nativeGetSampleRate();
    }

    public int getChannelCount() {
        return nativeGetChannelCount();
    }

    public int getAudioFormat() {
        return nativeGetAudioFormat();
    }

    public int getBufferSizeInBytes() {
        final int bufferSizeInMillis = nativeGetBufferSizeInMillis();
        final int sampleRate = nativeGetSampleRate();
        return (bufferSizeInMillis * sampleRate) / 1000;
    }

    public List<Voice> getAvailableVoices() {
        final List<Voice> voices = new LinkedList<Voice>();
        final String[] results = nativeGetAvailableVoices();

        for (int i = 0; i < results.length; i += 4) {
            final String name = results[i];
            final String identifier = results[i + 1];
            final int gender = Integer.parseInt(results[i + 2]);
            final int age = Integer.parseInt(results[i + 3]);
            final Voice voice = new Voice(name, identifier, gender, age);

            voices.add(voice);
        }

        return voices;
    }

    public void setVoiceByProperties(
            String name, String languages, int gender, int age, int variant) {
        nativeSetVoiceByProperties(name, languages, gender, age, variant);
    }

    public void setLanguage(String language, int variant) {
        attemptInit();

        nativeSetLanguage(language, variant);
    }

    public void setRate(int rate) {
        nativeSetRate(rate);
    }

    public void setPitch(int pitch) {
        nativeSetPitch(pitch);
    }

    public void synthesize(String text) {
        nativeSynthesize(text);
    }

    public void stop() {
        nativeStop();
    }

    @SuppressWarnings("unused")
    private void nativeSynthCallback(byte[] audioData) {
        if (mCallback == null)
            return;

        if (audioData == null) {
            mCallback.onSynthDataComplete();
        } else {
            mCallback.onSynthDataReady(audioData);
        }
    }

    private void attemptInit() {
        if (mInitialized) {
            return;
        }

        if (!CheckVoiceData.hasBaseResources(mContext)) {
            Log.e(TAG, "Missing base resources");
            return;
        }

        if (!nativeCreate(mDatapath)) {
            Log.e(TAG, "Failed to initialize speech synthesis library");
            return;
        }

        Log.i(TAG, "Initialized synthesis library with sample rate = " + getSampleRate());

        mInitialized = true;
    }

    private int mNativeData;

    private static native final boolean nativeClassInit();

    private native final boolean nativeCreate(String path);

    private native final boolean nativeDestroy();

    private native final int nativeGetSampleRate();

    private native final int nativeGetChannelCount();

    private native final int nativeGetAudioFormat();

    private native final int nativeGetBufferSizeInMillis();

    private native final String[] nativeGetAvailableVoices();

    private native final boolean nativeSetVoiceByProperties(
            String name, String languages, int gender, int age, int variant);

    private native final boolean nativeSetLanguage(String language, int variant);

    private native final boolean nativeSetRate(int rate);

    private native final boolean nativeSetPitch(int pitch);

    private native final boolean nativeSynthesize(String text);

    private native final boolean nativeStop();

    public interface SynthReadyCallback {
        void onSynthDataReady(byte[] audioData);

        void onSynthDataComplete();
    }

    public class Voice {
        public final String name;
        public final String identifier;
        public final int gender;
        public final int age;
        public final Locale locale;

        public Voice(String name, String identifier, int gender, int age) {
            this.name = name;
            this.identifier = identifier;
            this.gender = gender;
            this.age = age;

            locale = new Locale(name);
        }

        /**
         * Attempts a partial match against a query locale.
         *
         * @param query The locale to match.
         * @return A text-to-speech availability code. One of:
         *         <ul>
         *         <li>{@link TextToSpeech#LANG_NOT_SUPPORTED}
         *         <li>{@link TextToSpeech#LANG_AVAILABLE}
         *         <li>{@link TextToSpeech#LANG_COUNTRY_AVAILABLE}
         *         <li>{@link TextToSpeech#LANG_COUNTRY_VAR_AVAILABLE}
         *         </ul>
         */
        public int match(Locale query) {
            if (!locale.getISO3Language().equals(query.getISO3Language())) {
                return TextToSpeech.LANG_NOT_SUPPORTED;
            } else if (!locale.getISO3Country().equals(query.getISO3Country())) {
                return TextToSpeech.LANG_AVAILABLE;
            } else if (!locale.getVariant().equals(query.getVariant())) {
                return TextToSpeech.LANG_COUNTRY_AVAILABLE;
            } else {
                return TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE;
            }
        }

        @Override
        public String toString() {
            return name;
        }
    }
}
