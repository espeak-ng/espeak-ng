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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.media.AudioTrack;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.speech.tts.SynthesisCallback;
import android.speech.tts.SynthesisRequest;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeechService;
import android.text.TextUtils;
import android.util.Log;

import com.googlecode.eyesfree.espeak.SpeechSynthesis.SynthReadyCallback;
import com.googlecode.eyesfree.espeak.SpeechSynthesis.Voice;

import java.util.List;
import java.util.Locale;

/**
 * Implements the eSpeak engine as a {@link TextToSpeechService}.
 *
 * @author alanv@google.com (Alan Viverette)
 */
public class TtsService extends TextToSpeechService {
    private static final String TAG = TtsService.class.getSimpleName();
    private static final boolean DEBUG = false;

    private static final String DEFAULT_LANGUAGE = "en";
    private static final String DEFAULT_COUNTRY = "uk";
    private static final String DEFAULT_VARIANT = "";

    private SpeechSynthesis mEngine;
    private SynthesisCallback mCallback;

    private List<Voice> mAvailableVoices;

    private String mLanguage = DEFAULT_LANGUAGE;
    private String mCountry = DEFAULT_COUNTRY;
    private String mVariant = DEFAULT_VARIANT;

    @Override
    public void onCreate() {
        if (!CheckVoiceData.hasBaseResources(this)) {
            final IntentFilter filter =
                    new IntentFilter(DownloadVoiceData.BROADCAST_LANGUAGES_UPDATED);
            registerReceiver(mBroadcastReceiver, filter);

            final Intent intent = new Intent(this, DownloadVoiceData.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);

            return;
        }

        initializeTtsEngine();

        // This calls onIsLanguageAvailable() and must run AFTER initialization!
        super.onCreate();
    }

    /**
     * Sets up the native eSpeak engine.
     */
    private void initializeTtsEngine() {
        if (mEngine != null) {
            mEngine.stop();
            mEngine = null;
        }

        mEngine = new SpeechSynthesis(this, mSynthCallback);
        mAvailableVoices = mEngine.getAvailableVoices();
    }

    @Override
    protected String[] onGetLanguage() {
        // This is used to specify the language requested from GetSampleText.
        return new String[] {
                mLanguage, mCountry, mVariant
        };
    }

    @Override
    protected int onIsLanguageAvailable(String language, String country, String variant) {
        if (mAvailableVoices == null) {
            Log.e(TAG, "Attempted to check language availability before loading voices!");
            return TextToSpeech.LANG_NOT_SUPPORTED;
        }

        final Locale query = new Locale(language, country, variant);

        boolean hasLanguage = false;
        boolean hasCountry = false;

        synchronized (mAvailableVoices) {
            for (Voice voice : mAvailableVoices) {
                switch (voice.match(query)) {
                    case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE:
                        return TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE;
                    case TextToSpeech.LANG_COUNTRY_AVAILABLE:
                        hasCountry = true;
                    case TextToSpeech.LANG_AVAILABLE:
                        hasLanguage = true;
                        break;
                }
            }
        }

        if (!hasLanguage) {
            return TextToSpeech.LANG_NOT_SUPPORTED;
        } else if (!hasCountry) {
            return TextToSpeech.LANG_AVAILABLE;
        } else {
            return TextToSpeech.LANG_COUNTRY_AVAILABLE;
        }

    }

    @Override
    protected int onLoadLanguage(String language, String country, String variant) {
        final int result = onIsLanguageAvailable(language, country, variant);

        // Return immediately if the language is not available.
        if (result != TextToSpeech.LANG_AVAILABLE && result != TextToSpeech.LANG_COUNTRY_AVAILABLE
                && result != TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE) {
            Log.e(TAG, "Failed to load language {language='" + language + "', country='" + country
                    + "', variant='" + variant + "'");
            return result;
        }

        synchronized (this) {
            mLanguage = language;
            mCountry = ((country == null) ? "" : country);
            mVariant = ((variant == null) ? "" : variant);
        }

        return result;
    }

    @Override
    protected void onStop() {
        Log.i(TAG, "Received stop request.");

        mEngine.stop();
    }

    @Override
    protected synchronized void onSynthesizeText(
            SynthesisRequest request, SynthesisCallback callback) {
        final String text = request.getText();
        final String language = getRequestLanguage(request);
        final int gender = getDefaultGender();
        final int rate = scaleRate(request.getSpeechRate());
        final int pitch = scalePitch(request.getPitch());
        final Bundle params = request.getParams();

        mLanguage = request.getLanguage();
        mCountry = request.getCountry();
        mVariant = request.getVariant();

        if (DEBUG) {
            Log.i(TAG, "Received synthesis request: {language=\"" + language + "\"}");

            for (String key : params.keySet()) {
                Log.v(TAG,
                        "Synthesis request contained param {" + key + ", " + params.get(key) + "}");
            }
        }

        mCallback = callback;
        mCallback.start(mEngine.getSampleRate(), mEngine.getAudioFormat(),
                mEngine.getChannelCount());

        mEngine.setVoiceByProperties(null, language, gender, 0, 0);
        mEngine.setRate(rate);
        mEngine.setPitch(pitch);
        mEngine.synthesize(text);
    }

    /**
     * Scales the pitch by the user-specified value.
     *
     * @param pitch A pitch value.
     * @return A scaled pitch value.
     */
    private int scalePitch(int pitch) {
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        final String defaultPitchString = prefs.getString("default_pitch", "100");
        final int defaultPitch = Integer.parseInt(defaultPitchString);

        return (pitch * defaultPitch / 100);
    }

    /**
     * Returns user-specified gender.
     *
     * @return A gender value.
     */
    private int getDefaultGender() {
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        final String defaultGenderString = prefs.getString("default_gender", "0");
        final int defaultGender = Integer.parseInt(defaultGenderString);

        return defaultGender;
    }

    /**
     * Scales the rate by the user-specified value.
     *
     * @param rate A rate value.
     * @return A scaled rate value.
     */
    private int scaleRate(int rate) {
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        final String defaultRateString = prefs.getString("default_rate", "100");
        final int defaultRate = Integer.parseInt(defaultRateString);

        return (rate * defaultRate / 100);
    }

    /**
     * Retrieves the language code from a synthesis request.
     *
     * @param request The synthesis request.
     * @return A language code in the format "en-uk-n".
     */
    private static String getRequestLanguage(SynthesisRequest request) {
        final StringBuffer result = new StringBuffer(request.getLanguage());

        final String country = request.getCountry();
        final String variant = request.getVariant();

        if (!TextUtils.isEmpty(country)) {
            result.append('-');
            result.append(country);
        }

        if (!TextUtils.isEmpty(variant)) {
            result.append('-');
            result.append(variant);
        }

        return result.toString();
    }

    /**
     * Pipes synthesizer output from native eSpeak to an {@link AudioTrack}.
     */
    private final SpeechSynthesis.SynthReadyCallback mSynthCallback = new SynthReadyCallback() {
        @Override
        public void onSynthDataReady(byte[] audioData) {
            if ((audioData == null) || (audioData.length == 0)) {
                onSynthDataComplete();
                return;
            }

            final int maxBytesToCopy = mCallback.getMaxBufferSize();

            int offset = 0;

            while (offset < audioData.length) {
                final int bytesToWrite = Math.min(maxBytesToCopy, (audioData.length - offset));
                mCallback.audioAvailable(audioData, offset, bytesToWrite);
                offset += bytesToWrite;
            }
        }

        @Override
        public void onSynthDataComplete() {
            mCallback.done();
        }
    };

    /**
     * Listens for language update broadcasts and initializes the eSpeak engine.
     */
    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            initializeTtsEngine();
        }
    };
}
