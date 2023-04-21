/*
 * Copyright (C) 2012-2015 Reece H. Dunn
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

import android.os.Build;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.test.AndroidTestCase;
import android.util.Log;

import java.util.Locale;

import static com.reecedunn.espeak.test.TtsMatcher.isTtsLangCode;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class TextToSpeechTestCase extends AndroidTestCase
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
            // Wait until the text-to-speech engine is initialised (max: 20 seconds):

            mEngine = new TextToSpeech(getContext(), mInitCallback);
            for (int count = 0; !mInitialised && count < (4 * 20); ++count)
            {
                Thread.sleep(250);
            }

            assertThat(mInitialised, is(true));
            assertThat(mStatus, is(TextToSpeech.SUCCESS));
            assertThat(mEngine.getDefaultEngine(), is("com.reecedunn.espeak"));

            // Ensure that the voice data is installed (max: 20 seconds):

            Locale en = new Locale("en");
            int available = mEngine.isLanguageAvailable(en);
            for (int count = 0; available != TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE && count < (4 * 20); ++count)
            {
                Thread.sleep(250);
                available = mEngine.isLanguageAvailable(en);
                Log.d("TextToSpeechTestCase", "setUp: available = " + available);
            }

            assertThat(available, isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
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

    public TextToSpeech getEngine()
    {
        return mEngine;
    }

    @SuppressWarnings("deprecation")
    public static Locale getLanguage(TextToSpeech engine) {
        if (engine != null) {
            return engine.getLanguage();
        }
        return null;
    }
}
