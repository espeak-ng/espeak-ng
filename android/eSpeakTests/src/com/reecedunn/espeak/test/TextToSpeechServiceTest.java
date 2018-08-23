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
import android.os.Build;
import android.speech.tts.TextToSpeech;
import android.test.AndroidTestCase;

import com.reecedunn.espeak.TtsService;
import com.reecedunn.espeak.Voice;

import java.util.Locale;
import java.util.Set;

import static com.reecedunn.espeak.test.TtsMatcher.isTtsLangCode;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class TextToSpeechServiceTest extends AndroidTestCase
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

    @Override
    public void setUp() throws Exception
    {
        mService = new TtsServiceTest(getContext());
        mService.onCreate();
    }

    @Override
    public void tearDown()
    {
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

    public void testOnLoadLanguage() {
        assertThat(mService.onLoadLanguage("eng", "", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb"));

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
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb"));

        assertThat(mService.onLoadLanguage("eng", "FRA", "rp"), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb"));

        assertThat(mService.onLoadLanguage("eng", "FRA", ""), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb"));

        assertThat(mService.onLoadLanguage("ine", "", ""), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        checkLanguage(mService.onGetLanguage(), "eng", "GBR", "");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("en-gb"));
    }

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

    public void testOnGetDefaultVoiceNameFor() {
        assertThat(mService.onLoadLanguage("vie", "VNM", "central"), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "", ""), is("en-gb"));
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

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "", "scotland"), is("en-gb"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "FRA", "rp"), is("en-gb"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("eng", "FRA", ""), is("en-gb"));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));

        assertThat(mService.onGetDefaultVoiceNameFor("ine", "", ""), is(nullValue()));
        checkLanguage(mService.onGetLanguage(), "vie", "VNM", "central");
        assertThat(mService.getActiveVoice(), is(notNullValue()));
        assertThat(mService.getActiveVoice().name, is("vi-vn-x-central"));
    }

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
}
