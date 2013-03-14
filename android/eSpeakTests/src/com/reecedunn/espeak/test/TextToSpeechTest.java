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

import java.util.Locale;

import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.test.AndroidTestCase;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class TextToSpeechTest extends AndroidTestCase
{
    private TextToSpeech mEngine = null;
    private boolean mInitialised = false;
    private int mStatus = TextToSpeech.ERROR;

    private OnInitListener mInitCallback = new OnInitListener()
    {
        @Override
        public void onInit(int status)
        {
            mStatus = status;
            mInitialised = true;
        }
    };

    @Override
    public void setUp() throws Exception
    {
        try
        {
            // Wait until the text-to-speech engine is initialised:
            mEngine = new TextToSpeech(getContext(), mInitCallback);
            for (int count = 0; !mInitialised && count < 20; ++count)
            {
                Thread.sleep(250);
            }

            assertThat(mInitialised, is(true));
            assertThat(mStatus, is(TextToSpeech.SUCCESS));
            assertThat(mEngine.getDefaultEngine(), is("com.reecedunn.espeak"));
        }
        catch (Exception e)
        {
            tearDown();
            throw e;
        }
    }

    @Override
    public void tearDown()
    {
        if (mEngine != null)
        {
            mEngine.shutdown();
            mEngine = null;
        }
    }

    public void testUnsupportedLanguage()
    {
        assertThat(mEngine, is(notNullValue()));

        Locale initialLocale = mEngine.getLanguage();
        assertThat(mEngine.isLanguageAvailable(new Locale("cel")), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(mEngine.getLanguage().getLanguage(), is(initialLocale.getLanguage()));
        assertThat(mEngine.getLanguage().getCountry(), is(initialLocale.getCountry()));
        assertThat(mEngine.getLanguage().getVariant(), is(initialLocale.getVariant()));
    }

    public void testLanguages()
    {
        assertThat(mEngine, is(notNullValue()));

        for (VoiceData.Voice data : VoiceData.voices)
        {
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

                assertThat(mEngine.isLanguageAvailable(iana1), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(mEngine.setLanguage(iana1), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(data.javaCountry));
                assertThat(mEngine.getLanguage().getVariant(), is(data.variant));

                assertThat(mEngine.isLanguageAvailable(iana2), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(mEngine.setLanguage(iana2), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(data.javaCountry));
                assertThat(mEngine.getLanguage().getVariant(), is(""));

                assertThat(mEngine.isLanguageAvailable(iana3), is(TextToSpeech.LANG_AVAILABLE));
                assertThat(mEngine.setLanguage(iana3), is(TextToSpeech.LANG_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(""));
                assertThat(mEngine.getLanguage().getVariant(), is(""));

                assertThat(mEngine.isLanguageAvailable(java1), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(mEngine.setLanguage(java1), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(data.javaCountry));
                assertThat(mEngine.getLanguage().getVariant(), is(data.variant));

                assertThat(mEngine.isLanguageAvailable(java2), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(mEngine.setLanguage(java2), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(data.javaCountry));
                assertThat(mEngine.getLanguage().getVariant(), is(""));

                assertThat(mEngine.isLanguageAvailable(java3), is(TextToSpeech.LANG_AVAILABLE));
                assertThat(mEngine.setLanguage(java3), is(TextToSpeech.LANG_AVAILABLE));
                assertThat(mEngine.getLanguage().getLanguage(), is(data.javaLanguage));
                assertThat(mEngine.getLanguage().getCountry(), is(""));
                assertThat(mEngine.getLanguage().getVariant(), is(""));
            }
            catch (AssertionError e)
            {
                throw new VoiceData.Exception(data, null, e);
            }
        }
    }
}
