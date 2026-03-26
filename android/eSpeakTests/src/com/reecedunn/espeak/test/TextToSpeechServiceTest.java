/*
 * Copyright (C) 2015 Reece H. Dunn
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

package com.reecedunn.espeak.test;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.preference.PreferenceManager;
import android.speech.tts.TextToSpeech;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.reecedunn.espeak.LanguageSettings;
import com.reecedunn.espeak.TtsService;
import com.reecedunn.espeak.Voice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

import static com.reecedunn.espeak.test.TtsMatcher.isTtsLangCode;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

@RunWith(AndroidJUnit4.class)
public class TextToSpeechServiceTest
{
    public class TtsServiceTest extends TtsService
    {
        public TtsServiceTest(Context context)
        {
            attachBaseContext(context);
        }

        public String[] onGetLanguage() {
            return super.onGetLanguage();
        }

        public int onIsLanguageAvailable(String language, String country, String variant) {
            return super.onIsLanguageAvailable(language, country, variant);
        }

        public int onLoadLanguage(String language, String country, String variant) {
            return super.onLoadLanguage(language, country, variant);
        }

        public Set<String> onGetFeaturesForLanguage(String language, String country, String variant) {
            return super.onGetFeaturesForLanguage(language, country, variant);
        }

        public Voice getActiveVoice() {
            return mMatchingVoice;
        }

        public int selectLanguageWithFallback(String language, String country, String variant) {
            return super.selectLanguageWithFallback(language, country, variant);
        }

        @SuppressLint("NewApi")
        private android.speech.tts.Voice getVoice(String name) {
            for (android.speech.tts.Voice voice : onGetVoices()) {
                if (voice.getName().equals(name)) {
                    return voice;
                }
            }
            return null;
        }
    }

    private TtsServiceTest mService = null;
    private Context mContext = null;

    @Before
    public void setUp() throws Exception
    {
        mContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            mContext = mContext.createDeviceProtectedStorageContext();
        }
        mService = new TtsServiceTest(mContext);
        mService.onCreate();
    }

    @After
    public void tearDown()
    {
        if (mContext != null) {
            PreferenceManager.getDefaultSharedPreferences(mContext)
                    .edit()
                    .remove(LanguageSettings.PREF_SUPPORTED_LANGUAGES)
                    .apply();
        }
        if (mService != null)
        {
            mService.onDestroy();
            mService = null;
        }
    }

    private void checkLanguage(String[] locale, String language, String country, String variant) {
        assertThat(locale.length, is(3));
        assertThat(locale[0], is(language));
        assertThat(locale[1], is(country));
        assertThat(locale[2], is(variant));
    }

    @Test
    public void testOnLoadLanguage() {
        // When loading language-only ("eng"), the selected voice depends on
        // HashMap iteration order among matching en-gb voices.
        assertThat(mService.onLoadLanguage("eng", "", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, startsWith("en-gb"));
        String defaultEnName = mService.getActiveVoice().name;

        assertThat(mService.onLoadLanguage("eng", "USA", ""), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "USA", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-us"));

        assertThat(mService.onLoadLanguage("eng", "GBR", "scotland"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "scotland");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb-scotland"));

        assertThat(mService.onLoadLanguage("eng", "USA", "rp"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "USA", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-us"));

        assertThat(mService.onLoadLanguage("eng", "", "scotland"), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is(defaultEnName));

        assertThat(mService.onLoadLanguage("eng", "FRA", "rp"), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is(defaultEnName));

        assertThat(mService.onLoadLanguage("eng", "FRA", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is(defaultEnName));

        assertThat(mService.onLoadLanguage("ine", "", ""), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is(defaultEnName));
    }

    @Test
    public void testOnIsLanguageAvailable() {
        assertThat(mService.onLoadLanguage("vie", "VNM", "central"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));

        assertThat(mService.onIsLanguageAvailable("eng", "", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "USA", ""), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "GBR", "scotland"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "USA", "rp"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "", "scotland"), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "FRA", "rp"), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("eng", "FRA", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onIsLanguageAvailable("ine", "", ""), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));
    }

    @Test
    public void testOnGetDefaultVoiceNameFor() {
        assertThat(mService.onLoadLanguage("vie", "VNM", "central"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));

        // The default voice for language-only depends on HashMap iteration order.
        String defaultEn = mService.onGetDefaultVoiceNameFor("eng", "", "");
        assertThat(defaultEn, startsWith("en-gb"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "USA", ""), is("en-us"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "GBR", "scotland"), is("en-gb-scotland"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "USA", "rp"), is("en-us"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "", "scotland"), is(defaultEn));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "FRA", "rp"), is(defaultEn));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "FRA", ""), is(defaultEn));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("ine", "", ""), is(nullValue()));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));
    }

    @Test
    public void testLanguages() {
        for (VoiceData.Voice data : VoiceData.voices)
        {
            assertThat(mService.onIsLanguageAvailable(data.javaLanguage, data.javaCountry, data.variant), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
                assertThat(mService.onLoadLanguage(data.javaLanguage, data.javaCountry, data.variant), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                checkLanguage(mService.onGetLanguage(), data.javaLanguage, data.javaCountry, data.variant);
            } else {
                assertThat(mService.onGetDefaultVoiceNameFor(data.javaLanguage, data.javaCountry, data.variant), is(data.name));
                assertThat(mService.onLoadVoice(data.name), is(TextToSpeech.SUCCESS));

                android.speech.tts.Voice voice = mService.getVoice(data.name);
                assertThat(voice, is(notNullValue()));

                Locale locale = voice.getLocale();
                assertThat(locale, is(notNullValue()));
                assertThat(locale.getISO3Language(), is(data.javaLanguage));
                assertThat(locale.getISO3Country(), is(data.javaCountry));
                assertThat(locale.getVariant(), is(data.variant));

                Set<String> features = mService.onGetFeaturesForLanguage(data.javaLanguage, data.javaCountry, data.variant);
                assertThat(features, is(notNullValue()));
                assertThat(features.size(), is(0));
            }
        }
    }

    private void setFilteredLanguages(String... voiceIds) {
        Set<String> selected = new HashSet<>();
        for (String id : voiceIds) {
            selected.add(id);
        }
        PreferenceManager.getDefaultSharedPreferences(mContext)
                .edit()
                .putStringSet(LanguageSettings.PREF_SUPPORTED_LANGUAGES, selected)
                .commit();
    }

    @Test
    public void testOnIsLanguageAvailable_filteredFallback() {
        // Filter to Russian only.
        setFilteredLanguages("rus");

        // English is not in the filtered set, but Russian is available.
        // Should report LANG_AVAILABLE so screen readers don't skip this engine.
        assertThat(mService.onIsLanguageAvailable("eng", "", ""),
                isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
    }

    @Test
    public void testOnLoadLanguage_filteredFallback_freshStart() {
        // Filter to Russian only.
        setFilteredLanguages("rus");

        // Fresh start: no voice loaded yet, requesting English.
        // Should fall back to Russian and report LANG_AVAILABLE.
        assertThat(mService.onLoadLanguage("eng", "", ""),
                isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("ru"));
    }

    @Test
    public void testOnLoadLanguage_filteredFallback_reusesExistingVoice() {
        // Load Russian first.
        assertThat(mService.onLoadLanguage("rus", "", ""),
                isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(mService.getActiveVoice().name, is("ru"));

        // Now filter to Russian only.
        setFilteredLanguages("rus");

        // Request English — should keep the previously loaded Russian voice.
        assertThat(mService.onLoadLanguage("eng", "", ""),
                isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("ru"));
    }

    @Test
    public void testSelectLanguageWithFallback_filteredToRussianOnly() {
        // Filter to Russian only.
        setFilteredLanguages("rus");

        // Fresh start: requesting English should fall back to Russian.
        int result = mService.selectLanguageWithFallback("eng", "", "");
        assertThat(result, is(TextToSpeech.SUCCESS));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("ru"));
    }

    @Test
    public void testSelectLanguageWithFallback_supportedLanguageStillWorks() {
        // With all languages available, requesting English should work normally.
        int result = mService.selectLanguageWithFallback("eng", "", "");
        assertThat(result, is(TextToSpeech.SUCCESS));
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, startsWith("en-gb"));
    }
}
