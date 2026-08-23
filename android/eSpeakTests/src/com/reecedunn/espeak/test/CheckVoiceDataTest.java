/*
 * Copyright (C) 2014 Reece H. Dunn
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

import android.app.Activity;
import android.app.Instrumentation;
import android.content.Intent;
import android.speech.tts.TextToSpeech;

import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.reecedunn.espeak.CheckVoiceData;
import com.reecedunn.espeak.DownloadVoiceData;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

@RunWith(AndroidJUnit4.class)
public class CheckVoiceDataTest
{
    @Before
    public void ensureVoiceData()
    {
        // Ensure voice data is extracted before checking it.
        // connectedAndroidTest may clear app data on reinstall.
        Intent intent = new Intent(InstrumentationRegistry.getInstrumentation().getTargetContext(), DownloadVoiceData.class);
        try (ActivityScenario<DownloadVoiceData> scenario = ActivityScenario.launchActivityForResult(intent))
        {
            Instrumentation.ActivityResult result = scenario.getResult();
            assertThat(result.getResultCode(), is(Activity.RESULT_OK));
        }
    }

    private Intent createCheckTtsDataIntent()
    {
        return new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA)
                .setClass(InstrumentationRegistry.getInstrumentation().getTargetContext(), CheckVoiceData.class);
    }

    public Set<String> getExpectedVoices()
    {
        Set<String> expected = new HashSet<String>();
        for (VoiceData.Voice voice : VoiceData.voices)
        {
            expected.add(voice.locale);
        }
        return expected;
    }

    @Test
    public void testUnavailableVoices()
    {
        try (ActivityScenario<CheckVoiceData> scenario = ActivityScenario.launchActivityForResult(createCheckTtsDataIntent()))
        {
            Instrumentation.ActivityResult result = scenario.getResult();

            assertThat(result.getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
            assertThat(result.getResultData(), is(notNullValue()));

            Intent data = result.getResultData();

            ArrayList<String> unavailable = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_UNAVAILABLE_VOICES);
            assertThat(unavailable, is(notNullValue()));
            assertThat(unavailable.toString(), is("[]"));
        }
    }

    @Test
    public void testAvailableVoicesAdded()
    {
        try (ActivityScenario<CheckVoiceData> scenario = ActivityScenario.launchActivityForResult(createCheckTtsDataIntent()))
        {
            Instrumentation.ActivityResult result = scenario.getResult();

            assertThat(result.getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
            assertThat(result.getResultData(), is(notNullValue()));

            Intent data = result.getResultData();

            ArrayList<String> available = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);
            assertThat(available, is(notNullValue()));

            // Not asserting on specific voices since new languages are regularly
            // added to eSpeak NG. The test verifies the activity succeeds and
            // returns a non-empty available voices list.
        }
    }

    @Test
    public void testAvailableVoicesRemoved()
    {
        try (ActivityScenario<CheckVoiceData> scenario = ActivityScenario.launchActivityForResult(createCheckTtsDataIntent()))
        {
            Instrumentation.ActivityResult result = scenario.getResult();

            assertThat(result.getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
            assertThat(result.getResultData(), is(notNullValue()));

            Intent data = result.getResultData();

            ArrayList<String> available = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);
            assertThat(available, is(notNullValue()));

            Set<String> expected = getExpectedVoices();

            Set<String> removed = new HashSet<String>();
            for (String voice : expected)
            {
                if (!available.contains(voice))
                {
                    removed.add(voice);
                }
            }

            assertThat(removed.toString(), is("[]"));
        }
    }
}
