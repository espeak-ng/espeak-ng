/*
 * Copyright (C) 2012-2013 Reece H. Dunn
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

/*
 * This file implements the Android Text-to-Speech engine for eSpeak.
 *
 * Android Version: 4.0 (Ice Cream Sandwich)
 * API Version:     14
 */

package com.reecedunn.espeak;

import android.annotation.SuppressLint;
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
import android.util.Log;

import com.reecedunn.espeak.SpeechSynthesis.SynthReadyCallback;

import java.util.List;
import java.util.Locale;

/**
 * Implements the eSpeak engine as a {@link TextToSpeechService}.
 *
 * @author msclrhd@gmail.com (Reece H. Dunn)
 * @author alanv@google.com (Alan Viverette)
 */
@SuppressLint("NewApi")
public class TtsService extends TextToSpeechService {
    private static final String TAG = TtsService.class.getSimpleName();
    private static final boolean DEBUG = false;

    private static final String DEFAULT_LANGUAGE = "en";
    private static final String DEFAULT_COUNTRY = "uk";
    private static final String DEFAULT_VARIANT = "";

    private SpeechSynthesis mEngine;
    private SynthesisCallback mCallback;

    private List<Voice> mAvailableVoices;
    private Voice mMatchingVoice = null;

    private BroadcastReceiver mOnLanguagesDownloaded = null;
    private BroadcastReceiver mOnSystemLocaleChanged = null;

    private String mLanguage = DEFAULT_LANGUAGE;
    private String mCountry = DEFAULT_COUNTRY;
    private String mVariant = DEFAULT_VARIANT;

    @Override
    public void onCreate() {
        initializeTtsEngine();
        super.onCreate();

        mOnSystemLocaleChanged = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Locale locale = Locale.getDefault();
                int ret = onLoadLanguage(locale.getISO3Language(), locale.getISO3Country(), locale.getVariant());
                Log.i("LocaleChange", "Locale = " + locale + "; status = " + ret);
            }
        };

        final IntentFilter filter = new IntentFilter(Intent.ACTION_LOCALE_CHANGED);
        registerReceiver(mOnSystemLocaleChanged, filter);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mOnLanguagesDownloaded != null) {
            unregisterReceiver(mOnLanguagesDownloaded);
        }
        if (mOnSystemLocaleChanged != null) {
            unregisterReceiver(mOnSystemLocaleChanged);
        }
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
        if (!CheckVoiceData.hasBaseResources(this) || CheckVoiceData.canUpgradeResources(this)) {
            if (mOnLanguagesDownloaded == null) {
                mOnLanguagesDownloaded = new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        initializeTtsEngine();
                    }
                };

                final IntentFilter filter = new IntentFilter(DownloadVoiceData.BROADCAST_LANGUAGES_UPDATED);
                registerReceiver(mOnLanguagesDownloaded, filter);
            }

            final Intent intent = new Intent(this, DownloadVoiceData.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);

            return TextToSpeech.LANG_MISSING_DATA;
        }

        final Locale query = new Locale(language, country, variant);

        Voice languageVoice = null;
        Voice countryVoice = null;

        synchronized (mAvailableVoices) {
            for (Voice voice : mAvailableVoices) {
                switch (voice.match(query)) {
                    case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE:
                        mMatchingVoice = voice;
                        return TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE;
                    case TextToSpeech.LANG_COUNTRY_AVAILABLE:
                        countryVoice = voice;
                    case TextToSpeech.LANG_AVAILABLE:
                        languageVoice = voice;
                        break;
                }
            }
        }

        if (languageVoice == null) {
            mMatchingVoice = null;
            return TextToSpeech.LANG_NOT_SUPPORTED;
        } else if (countryVoice == null) {
            mMatchingVoice = languageVoice;
            return TextToSpeech.LANG_AVAILABLE;
        } else {
            mMatchingVoice = countryVoice;
            return TextToSpeech.LANG_COUNTRY_AVAILABLE;
        }
    }

    @Override
    protected int onLoadLanguage(String language, String country, String variant) {
        final int result = onIsLanguageAvailable(language, country, variant);
        switch (result) {
        case TextToSpeech.LANG_AVAILABLE:
            synchronized (this) {
                mLanguage = language;
                mCountry = "";
                mVariant = "";
            }
            break;
        case TextToSpeech.LANG_COUNTRY_AVAILABLE:
            synchronized (this) {
                mLanguage = language;
                mCountry = ((country == null) ? "" : country);
                mVariant = "";
            }
            break;
        case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE:
            synchronized (this) {
                mLanguage = language;
                mCountry = ((country == null) ? "" : country);
                mVariant = ((variant == null) ? "" : variant);
            }
            break;
        case TextToSpeech.LANG_NOT_SUPPORTED:
            Log.e(TAG, "Unsupported language {language='" + language + "', country='" + country
                    + "', variant='" + variant + "'}");
            break;
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
        final int result = onLoadLanguage(mLanguage, mCountry, mVariant);
        switch (result) {
        case TextToSpeech.LANG_MISSING_DATA:
        case TextToSpeech.LANG_NOT_SUPPORTED:
            return;
        }

        String text = request.getText();
        if (text == null)
            return;

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        final String variantString = prefs.getString("espeak_variant", null);
        final VoiceVariant variant;
        if (variantString == null) {
            final int gender = getPreferenceValue(prefs, "default_gender", SpeechSynthesis.GENDER_MALE);
            if (gender == SpeechSynthesis.GENDER_FEMALE) {
                variant = VoiceVariant.parseVoiceVariant("default-female");
            } else {
                variant = VoiceVariant.parseVoiceVariant("default-male");
            }
        } else {
            variant = VoiceVariant.parseVoiceVariant(variantString);
        }

        int rate = getPreferenceValue(prefs, "espeak_rate", Integer.MIN_VALUE);
        if (rate == Integer.MIN_VALUE) {
            // Try the old eyes-free setting:
            int defaultValue = mEngine.Rate.getDefaultValue();
            int maxValue = mEngine.Rate.getMaxValue();

            rate = (getPreferenceValue(prefs, "default_rate", 100) / 100) * defaultValue;
            if (rate < defaultValue) rate = defaultValue;
            if (rate > maxValue) rate = maxValue;
        }

        int pitch = getPreferenceValue(prefs, "espeak_pitch", Integer.MIN_VALUE);
        if (pitch == Integer.MIN_VALUE) {
            // Try the old eyes-free setting:
            pitch = getPreferenceValue(prefs, "default_pitch", 100) / 2;
        }

        if (DEBUG) {
            Log.i(TAG, "Received synthesis request: {language=\"" + mMatchingVoice.name + "\"}");

            final Bundle params = request.getParams();
            for (String key : params.keySet()) {
                Log.v(TAG,
                        "Synthesis request contained param {" + key + ", " + params.get(key) + "}");
            }
        }

        if (text.startsWith("<?xml"))
        {
            // eSpeak does not recognise/skip "<?...?>" preprocessing tags,
            // so need to remove these before passing to synthesize.
            text = text.substring(text.indexOf("?>") + 2).trim();
        }

        mCallback = callback;
        mCallback.start(mEngine.getSampleRate(), mEngine.getAudioFormat(),
                mEngine.getChannelCount());

        mEngine.setVoice(mMatchingVoice, variant);
        mEngine.Rate.setValue(rate, request.getSpeechRate());
        mEngine.Pitch.setValue(pitch, request.getPitch());
        mEngine.PitchRange.setValue(getPreferenceValue(prefs, "espeak_pitch_range", mEngine.PitchRange.getDefaultValue()));
        mEngine.Volume.setValue(getPreferenceValue(prefs, "espeak_volume", mEngine.Volume.getDefaultValue()));
        mEngine.synthesize(text, text.startsWith("<speak"));
    }

    private int getPreferenceValue(SharedPreferences prefs, String preference, int defaultValue) {
        final String prefString = prefs.getString(preference, null);
        if (prefString == null) {
            return defaultValue;
        }
        return Integer.parseInt(prefString);
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
}
