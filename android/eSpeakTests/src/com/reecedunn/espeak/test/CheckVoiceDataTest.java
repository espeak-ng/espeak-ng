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
import android.content.Intent;
import android.speech.tts.TextToSpeech;
import android.test.ActivityUnitTestCase;

import com.reecedunn.espeak.CheckVoiceData;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class CheckVoiceDataTest extends ActivityUnitTestCase<CheckVoiceData>
{
    Field mResultCode;
    Field mResultData;

    public void setUp() throws Exception
    {
        super.setUp();

        mResultCode = Activity.class.getDeclaredField("mResultCode");
        mResultCode.setAccessible(true);

        mResultData = Activity.class.getDeclaredField("mResultData");
        mResultData.setAccessible(true);
    }

    public int getResultCode() throws IllegalAccessException
    {
        return (Integer)mResultCode.get(getActivity());
    }

    public Intent getResultData() throws IllegalAccessException
    {
        return (Intent)mResultData.get(getActivity());
    }

    public CheckVoiceDataTest()
    {
        super(CheckVoiceData.class);
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

    public void testUnavailableVoices() throws IllegalAccessException
    {
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
        startActivity(intent, null, null);

        assertThat(getActivity(), is(notNullValue()));
        assertThat(isFinishCalled(), is(true));
        assertThat(getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
        assertThat(getResultData(), is(notNullValue()));

        Intent data = getResultData();

        ArrayList<String> unavailable = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_UNAVAILABLE_VOICES);
        assertThat(unavailable, is(notNullValue()));
        assertThat(unavailable.toString(), is("[]"));
    }

    public void testAvailableVoicesAdded() throws IllegalAccessException
    {
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
        startActivity(intent, null, null);

        assertThat(getActivity(), is(notNullValue()));
        assertThat(isFinishCalled(), is(true));
        assertThat(getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
        assertThat(getResultData(), is(notNullValue()));

        Intent data = getResultData();

        ArrayList<String> available = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);
        assertThat(available, is(notNullValue()));

        Set<String> expected = getExpectedVoices();

        Set<String> added = new HashSet<String>();
        for (String voice : available)
        {
            if (!expected.contains(voice))
            {
                added.add(voice);
            }
        }

        assertThat(added.toString(), is("[]"));
    }

    public void testAvailableVoicesRemoved() throws IllegalAccessException
    {
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
        startActivity(intent, null, null);

        assertThat(getActivity(), is(notNullValue()));
        assertThat(isFinishCalled(), is(true));
        assertThat(getResultCode(), is(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS));
        assertThat(getResultData(), is(notNullValue()));

        Intent data = getResultData();

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
