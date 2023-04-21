/*
 * Copyright (C) 2012-2015 Reece H. Dunn
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
 * This file implements the Java API to eSpeak using the JNI bindings.
 *
 * Android Version: 4.0 (Ice Cream Sandwich)
 * API Version:     14
 */

package com.reecedunn.espeak;

import android.content.Context;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.util.Log;

import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.MissingResourceException;

public class SpeechSynthesis {
    private static final String TAG = SpeechSynthesis.class.getSimpleName();

    public static final int GENDER_UNSPECIFIED = 0;
    public static final int GENDER_MALE = 1;
    public static final int GENDER_FEMALE = 2;

    public static final int AGE_ANY = 0;
    public static final int AGE_YOUNG = 12;
    public static final int AGE_OLD = 60;

    public static final int CHANNEL_COUNT_MONO = 1;
    public static final int FORMAT_PCM_S16 = 2;

    static {
        System.loadLibrary("ttsespeak");

        nativeClassInit();
    }

    private final Context mContext;
    private final SynthReadyCallback mCallback;
    private final String mDatapath;

    private boolean mInitialized = false;
    private static int mVoiceCount = 0;
    private int mSampleRate = 0;

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

    public static String getVersion() {
        return nativeGetVersion();
    }

    public static int getVoiceCount() {
        return mVoiceCount;
    }

    public int getSampleRate() {
        return mSampleRate;
    }

    public int getChannelCount() {
        return CHANNEL_COUNT_MONO;
    }

    public int getAudioFormat() {
        return FORMAT_PCM_S16;
    }

    private Locale getLocaleFromLanguageName(String name) {
        if (mLocaleFixes.containsKey(name)) {
            return mLocaleFixes.get(name);
        }
        String[] parts = name.split("-");
        switch (parts.length) {
            case 1: // language
                return new Locale(parts[0]);
            case 2: // language-country
                return new Locale(parts[0], parts[1]);
            case 3: // language-country-variant
                return new Locale(parts[0], parts[1], parts[2]);
            case 4: // language-country-x-privateuse
                return new Locale(parts[0], parts[1], parts[3]);
            default:
                return null;
        }
    }

    public List<Voice> getAvailableVoices() {
        final List<Voice> voices = new LinkedList<Voice>();
        final String[] results = nativeGetAvailableVoices();
        mVoiceCount = results.length / 4;

        for (int i = 0; i < results.length; i += 4) {
            final String name = results[i];
            final String identifier = results[i + 1];
            final int gender = Integer.parseInt(results[i + 2]);
            final int age = Integer.parseInt(results[i + 3]);

            try {
                final Locale locale;
                if (identifier.equals("asia/fa-en-us")) {
                    throw new IllegalArgumentException("Voice '" + identifier + "' is a duplicate voice.");
                } else {
                    locale = getLocaleFromLanguageName(name);
                    if (locale == null) {
                        throw new IllegalArgumentException("Locale not supported.");
                    }
                }

                String language = locale.getISO3Language();
                if (language.equals("")) {
                    throw new IllegalArgumentException("Language '" + locale.getLanguage() + "' not supported.");
                }

                String country  = locale.getISO3Country();
                if (country.equals("") && !locale.getCountry().equals("")) {
                    throw new IllegalArgumentException("Country '" + locale.getCountry() + "' not supported.");
                }

                final Voice voice = new Voice(name, identifier, gender, age, locale);
                voices.add(voice);
            } catch (MissingResourceException e) {
                // Android 4.3 throws this exception if the 3-letter language
                // (e.g. nci) or country (e.g. 021) code is missing for a locale.
                // Earlier versions return an empty string (handled above).
                Log.d(TAG, "getAvailableResources: skipping " + name + " => " + e.getMessage());
            } catch (IllegalArgumentException e) {
                Log.d(TAG, "getAvailableResources: skipping " + name + " => " + e.getMessage());
            }
        }

        return voices;
    }

    public void setVoice(Voice voice, VoiceVariant variant) {
        // NOTE: espeak_SetVoiceByProperties does not support specifying the
        // voice variant (e.g. klatt), but espeak_SetVoiceByName does.
        if (variant.variant == null) {
            nativeSetVoiceByProperties(voice.name, variant.gender, variant.age);
        } else {
            nativeSetVoiceByName(voice.identifier + "+" + variant.variant);
        }
    }

    public void setPunctuationCharacters(String characters) {
        nativeSetPunctuationCharacters(characters);
    }

    /** Don't announce any punctuation characters. */
    public static final int PUNCT_NONE = 0;

    /** Announce every punctuation character. */
    public static final int PUNCT_ALL = 1;

    /** Announce some of the punctuation characters. */
    public static final int PUNCT_SOME = 2;

    public enum UnitType {
        Percentage,
        WordsPerMinute,
        /** One of the PUNCT_* constants. */
        Punctuation,
    }

    public class Parameter {
        private final int id;
        private final int min;
        private final int max;
        private final UnitType unitType;

        private Parameter(int id, int min, int max, UnitType unitType) {
            this.id = id;
            this.min = min;
            this.max = max;
            this.unitType = unitType;
        }

        public int getMinValue() {
            return min;
        }

        public int getMaxValue() {
            return max;
        }

        public int getDefaultValue() {
            return nativeGetParameter(id, 0);
        }

        public int getValue() {
            return nativeGetParameter(id, 1);
        }

        public void setValue(int value, int scale) {
            setValue((value * scale) / 100);
        }

        public void setValue(int value) {
            nativeSetParameter(id, value);
        }

        public UnitType getUnitType() {
            return unitType;
        }
    }

    /** Speech rate. */
    public final Parameter Rate = new Parameter(1, 80, 449, UnitType.WordsPerMinute);

    /** Audio volume. */
    public final Parameter Volume = new Parameter(2, 0, 200, UnitType.Percentage);

    /** Base pitch. */
    public final Parameter Pitch = new Parameter(3, 0, 100, UnitType.Percentage);

    /** Pitch range (monotone = 0). */
    public final Parameter PitchRange = new Parameter(4, 0, 100, UnitType.Percentage);

    /** Which punctuation characters to announce. */
    public final Parameter Punctuation = new Parameter(5, 0, 2, UnitType.Punctuation);

    public void synthesize(String text, boolean isSsml) {
        nativeSynthesize(text, isSsml);
    }

    public void stop() {
        nativeStop();
    }

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

        mSampleRate = nativeCreate(mDatapath);
        if (mSampleRate == 0) {
            Log.e(TAG, "Failed to initialize speech synthesis library");
            return;
        }

        Log.i(TAG, "Initialized synthesis library with sample rate = " + getSampleRate());

        mInitialized = true;
    }

    public static String getSampleText(Context context, Locale locale) {
        final DisplayMetrics metrics = context.getResources().getDisplayMetrics();
        final Configuration config = context.getResources().getConfiguration();

        final String language = getIanaLanguageCode(locale.getLanguage());
        final String country = getIanaCountryCode(locale.getCountry());
        config.locale = new Locale(language, country, locale.getVariant());

        Resources res = new Resources(context.getAssets(), metrics, config);
        return res.getString(R.string.sample_text, config.locale.getDisplayName(config.locale));
    }

    private static native final boolean nativeClassInit();

    private native final int nativeCreate(String path);

    private native final static String nativeGetVersion();

    private native final String[] nativeGetAvailableVoices();

    private native final boolean nativeSetVoiceByName(String name);

    private native final boolean nativeSetVoiceByProperties(String language, int gender, int age);

    private native final boolean nativeSetParameter(int parameter, int value);

    private native final int nativeGetParameter(int parameter, int current);

    private native final boolean nativeSetPunctuationCharacters(String characters);

    private native final boolean nativeSynthesize(String text, boolean isSsml);

    private native final boolean nativeStop();

    public interface SynthReadyCallback {
        void onSynthDataReady(byte[] audioData);

        void onSynthDataComplete();
    }

    public static String getIanaLanguageCode(String code) {
        return getIanaLocaleCode(code, mJavaToIanaLanguageCode);
    }

    public static String getIanaCountryCode(String code) {
        return getIanaLocaleCode(code, mJavaToIanaCountryCode);
    }

    private static String getIanaLocaleCode(String code, final Map<String, String> javaToIana) {
        final String iana = javaToIana.get(code);
        if (iana != null) {
            return iana;
        }
        return code;
    }

    private static final Map<String, String> mJavaToIanaLanguageCode = new HashMap<String, String>();
    private static final Map<String, String> mJavaToIanaCountryCode = new HashMap<String, String>();
    private static final HashMap<String, Locale> mLocaleFixes = new HashMap<String, Locale>();
    static {
        mJavaToIanaLanguageCode.put("afr", "af");
        mJavaToIanaLanguageCode.put("amh", "am");
        mJavaToIanaLanguageCode.put("ara", "ar");
        mJavaToIanaLanguageCode.put("arg", "an");
        mJavaToIanaLanguageCode.put("asm", "as");
        mJavaToIanaLanguageCode.put("aze", "az");
        mJavaToIanaLanguageCode.put("bul", "bg");
        mJavaToIanaLanguageCode.put("ben", "bn");
        mJavaToIanaLanguageCode.put("bos", "bs");
        mJavaToIanaLanguageCode.put("cat", "ca");
        mJavaToIanaLanguageCode.put("ces", "cs");
        mJavaToIanaLanguageCode.put("cym", "cy");
        mJavaToIanaLanguageCode.put("dan", "da");
        mJavaToIanaLanguageCode.put("deu", "de");
        mJavaToIanaLanguageCode.put("ell", "el");
        mJavaToIanaLanguageCode.put("eng", "en");
        mJavaToIanaLanguageCode.put("epo", "eo");
        mJavaToIanaLanguageCode.put("spa", "es");
        mJavaToIanaLanguageCode.put("est", "et");
        mJavaToIanaLanguageCode.put("eus", "eu");
        mJavaToIanaLanguageCode.put("fas", "fa");
        mJavaToIanaLanguageCode.put("fin", "fi");
        mJavaToIanaLanguageCode.put("fra", "fr");
        mJavaToIanaLanguageCode.put("gle", "ga");
        mJavaToIanaLanguageCode.put("gla", "gd");
        mJavaToIanaLanguageCode.put("grn", "gn");
        mJavaToIanaLanguageCode.put("guj", "gu");
        mJavaToIanaLanguageCode.put("hin", "hi");
        mJavaToIanaLanguageCode.put("hrv", "hr");
        mJavaToIanaLanguageCode.put("hun", "hu");
        mJavaToIanaLanguageCode.put("hye", "hy");
        mJavaToIanaLanguageCode.put("ina", "ia");
        mJavaToIanaLanguageCode.put("ind", "in"); // NOTE: The deprecated 'in' code is used by Java/Android.
        mJavaToIanaLanguageCode.put("isl", "is");
        mJavaToIanaLanguageCode.put("ita", "it");
        mJavaToIanaLanguageCode.put("jpn", "ja");
        mJavaToIanaLanguageCode.put("kat", "ka");
        mJavaToIanaLanguageCode.put("kal", "kl");
        mJavaToIanaLanguageCode.put("kan", "kn");
        mJavaToIanaLanguageCode.put("kir", "ky");
        mJavaToIanaLanguageCode.put("kor", "ko");
        mJavaToIanaLanguageCode.put("kur", "ku");
        mJavaToIanaLanguageCode.put("lat", "la");
        mJavaToIanaLanguageCode.put("lit", "lt");
        mJavaToIanaLanguageCode.put("lav", "lv");
        mJavaToIanaLanguageCode.put("mkd", "mk");
        mJavaToIanaLanguageCode.put("mal", "ml");
        mJavaToIanaLanguageCode.put("mar", "mr");
        mJavaToIanaLanguageCode.put("mlt", "mt");
        mJavaToIanaLanguageCode.put("mri", "mi");
        mJavaToIanaLanguageCode.put("msa", "ms");
        mJavaToIanaLanguageCode.put("mya", "my");
        mJavaToIanaLanguageCode.put("nep", "ne");
        mJavaToIanaLanguageCode.put("nld", "nl");
        mJavaToIanaLanguageCode.put("nob", "nb");
        mJavaToIanaLanguageCode.put("nor", "no");
        mJavaToIanaLanguageCode.put("ori", "or");
        mJavaToIanaLanguageCode.put("orm", "om");
        mJavaToIanaLanguageCode.put("pan", "pa");
        mJavaToIanaLanguageCode.put("pol", "pl");
        mJavaToIanaLanguageCode.put("por", "pt");
        mJavaToIanaLanguageCode.put("ron", "ro");
        mJavaToIanaLanguageCode.put("rus", "ru");
        mJavaToIanaLanguageCode.put("sin", "si");
        mJavaToIanaLanguageCode.put("slk", "sk");
        mJavaToIanaLanguageCode.put("slv", "sl");
        mJavaToIanaLanguageCode.put("snd", "sd");
        mJavaToIanaLanguageCode.put("sqi", "sq");
        mJavaToIanaLanguageCode.put("srp", "sr");
        mJavaToIanaLanguageCode.put("swe", "sv");
        mJavaToIanaLanguageCode.put("swa", "sw");
        mJavaToIanaLanguageCode.put("tam", "ta");
        mJavaToIanaLanguageCode.put("tel", "te");
        mJavaToIanaLanguageCode.put("tat", "tt");
        mJavaToIanaLanguageCode.put("tsn", "tn");
        mJavaToIanaLanguageCode.put("tur", "tr");
        mJavaToIanaLanguageCode.put("urd", "ur");
        mJavaToIanaLanguageCode.put("vie", "vi");
        mJavaToIanaLanguageCode.put("zho", "zh");

        mJavaToIanaCountryCode.put("ARM", "AM");
        mJavaToIanaCountryCode.put("BEL", "BE");
        mJavaToIanaCountryCode.put("BRA", "BR");
        mJavaToIanaCountryCode.put("CHE", "CH");
        mJavaToIanaCountryCode.put("FRA", "FR");
        mJavaToIanaCountryCode.put("GBR", "GB");
        mJavaToIanaCountryCode.put("HKG", "HK");
        mJavaToIanaCountryCode.put("JAM", "JM");
        mJavaToIanaCountryCode.put("MEX", "MX");
        mJavaToIanaCountryCode.put("PRT", "PT");
        mJavaToIanaCountryCode.put("USA", "US");
        mJavaToIanaCountryCode.put("VNM", "VN");

        // Fix up BCP47 locales not handled correctly by Android:
        mLocaleFixes.put("cmn", new Locale("zh"));
        mLocaleFixes.put("en-029", new Locale("en", "JM"));
        mLocaleFixes.put("es-419", new Locale("es", "MX"));
        mLocaleFixes.put("hy-arevmda", new Locale("hy", "AM", "arevmda")); // hy-arevmda crashes on Android 5.0
        mLocaleFixes.put("yue", new Locale("zh", "HK"));
    }
}
