/*
 * Copyright (C) 2013 Reece H. Dunn
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

import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.test.AndroidTestCase;

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.VoiceSettings;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class VoiceSettingsTest extends AndroidTestCase
{
    // No Settings (New Install)

    public void testNoPreferences()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
    }

    // Old Settings

    public void testDefaultGenderMale()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_gender", Integer.toString(SpeechSynthesis.GENDER_MALE));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
    }

    public void testDefaultGenderFemale()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_gender", Integer.toString(SpeechSynthesis.GENDER_FEMALE));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs);
        assertThat(settings.getVoiceVariant().toString(), is("female"));
    }

    // New Settings

    public void testEspeakVariant()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_variant", "klatt2-old");
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs);
        assertThat(settings.getVoiceVariant().toString(), is("klatt2-old"));
    }

    // Mixed (Old and New) Settings

    public void testEspeakVariantWithDefaultGenderFemale()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_gender", Integer.toString(SpeechSynthesis.GENDER_FEMALE));
        editor.putString("espeak_variant", "klatt4");
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs);
        assertThat(settings.getVoiceVariant().toString(), is("klatt4"));
    }
}
