/*
 * Copyright (C) 2012-2013 Reece H. Dunn
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

import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

import android.annotation.SuppressLint;
import android.os.Build;
import android.speech.tts.TextToSpeech;

import static com.reecedunn.espeak.test.TtsMatcher.isTtsLangCode;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class TextToSpeechTest extends TextToSpeechTestCase
{
    private Set<Object> mVoices = null;
    private Set<String> mAdded = new HashSet<String>();
    private Set<String> mRemoved = new HashSet<String>();

    @SuppressLint("NewApi")
    public Set<Object> getVoices()
    {
        if (mVoices == null)
        {
            Set<android.speech.tts.Voice> voiceData = getEngine().getVoices();
            assertThat(voiceData, is(notNullValue()));

            mVoices = new HashSet<Object>();
            for (android.speech.tts.Voice voice : voiceData)
            {
                mVoices.add(voice);
            }

            Set<String> voices = new HashSet<String>();
            for (Object data : mVoices)
            {
                voices.add(((android.speech.tts.Voice)data).getName());
            }

            Set<String> expected = new HashSet<String>();
            for (VoiceData.Voice data : VoiceData.voices)
            {
                expected.add(data.name);
            }

            for (String voice : voices)
            {
                if (!expected.contains(voice))
                {
                    mAdded.add(voice);
                }
            }

            for (String voice : expected)
            {
                if (!voices.contains(voice))
                {
                    mRemoved.add(voice);
                }
            }
        }
        return mVoices;
    }

    public void testAddedVoices()
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP)
            return;

        getVoices(); // Ensure that the voice data has been populated.
        assertThat(mAdded.toString(), is("[]"));
    }

    public void testRemovedVoices()
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP)
            return;

        getVoices(); // Ensure that the voice data has been populated.
        assertThat(mRemoved.toString(), is("[]"));
    }

    public void testUnsupportedLanguage()
    {
        assertThat(getEngine(), is(notNullValue()));

        Locale initialLocale = getLanguage(getEngine());
        assertThat(getEngine().isLanguageAvailable(new Locale("cel")), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        Locale language = getLanguage(getEngine());

        assertThat(getLanguage(getEngine()).getLanguage(), is(initialLocale.getLanguage()));
        assertThat(getLanguage(getEngine()).getCountry(), is(initialLocale.getCountry()));
        assertThat(getLanguage(getEngine()).getVariant(), is(initialLocale.getVariant()));
    }

    public void testLanguages()
    {
        assertThat(getEngine(), is(notNullValue()));

        for (VoiceData.Voice data : VoiceData.voices)
        {
            String context = null;
            try
            {
                // Skip the voice if the language code is not supported by Android:
                if (data.javaLanguage.equals("")) continue;

                final Locale iana1 = new Locale(data.ianaLanguage, data.ianaCountry, data.variant);
                final Locale iana2 = new Locale(data.ianaLanguage, data.ianaCountry, "test");
                final Locale iana3 = new Locale(data.ianaLanguage, "VU", data.variant);

                final Locale java1 = new Locale(data.javaLanguage, data.javaCountry, data.variant);
                final Locale java2 = new Locale(data.javaLanguage, data.javaCountry, "test");
                final Locale java3 = new Locale(data.javaLanguage, "VUT", data.variant);

                context = "iana1";
                assertThat(getEngine().isLanguageAvailable(iana1), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(getEngine().setLanguage(iana1), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(data.javaCountry));
                assertThat(getLanguage(getEngine()).getVariant(), is(data.variant));

                context = "iana2";
                assertThat(getEngine().isLanguageAvailable(iana2), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(getEngine().setLanguage(iana2), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(data.javaCountry));
                assertThat(getLanguage(getEngine()).getVariant(), is(""));

                context = "iana3";
                assertThat(getEngine().isLanguageAvailable(iana3), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
                assertThat(getEngine().setLanguage(iana3), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(""));
                assertThat(getLanguage(getEngine()).getVariant(), is(""));

                context = "java1";
                assertThat(getEngine().isLanguageAvailable(java1), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(getEngine().setLanguage(java1), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(data.javaCountry));
                assertThat(getLanguage(getEngine()).getVariant(), is(data.variant));

                context = "java2";
                assertThat(getEngine().isLanguageAvailable(java2), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(getEngine().setLanguage(java2), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(data.javaCountry));
                assertThat(getLanguage(getEngine()).getVariant(), is(""));

                context = "java3";
                assertThat(getEngine().isLanguageAvailable(java3), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
                assertThat(getEngine().setLanguage(java3), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
                assertThat(getLanguage(getEngine()).getLanguage(), is(data.javaLanguage));
                assertThat(getLanguage(getEngine()).getCountry(), is(""));
                assertThat(getLanguage(getEngine()).getVariant(), is(""));
            }
            catch (AssertionError e)
            {
                throw new VoiceData.Exception(data, context, e);
            }
        }
    }
}
