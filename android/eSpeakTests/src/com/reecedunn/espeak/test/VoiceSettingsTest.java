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

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.VoiceSettings;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class VoiceSettingsTest extends TextToSpeechTestCase
{
    private SpeechSynthesis.SynthReadyCallback mCallback = new SpeechSynthesis.SynthReadyCallback()
    {
        @Override
        public void onSynthDataReady(byte[] audioData)
        {
        }

        @Override
        public void onSynthDataComplete()
        {
        }
    };

    // No Settings (New Install)

    public void testNoPreferences()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    // Old Settings

    public void testDefaultGenderMale()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_gender", Integer.toString(SpeechSynthesis.GENDER_MALE));
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void testDefaultGenderFemale()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_gender", Integer.toString(SpeechSynthesis.GENDER_FEMALE));
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("female"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void defaultRateTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_rate", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(settingValue));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void testDefaultRate()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        defaultRateTest(300, 450, synth); // clamped to maximum value
        defaultRateTest(200, 350, synth);
        defaultRateTest(100, 175, synth); // default value
        defaultRateTest( 50,  87, synth);
        defaultRateTest( 25,  80, synth); // clamped to minimum value
    }

    public void defaultPitchTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_pitch", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(settingValue));
    }

    public void testDefaultPitch()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        defaultPitchTest(250, 100, synth); // clamped to maximum value
        defaultPitchTest(200, 100, synth);
        defaultPitchTest(100,  50, synth); // default value
        defaultPitchTest( 50,  25, synth);
        defaultPitchTest(  0,   0, synth);
        defaultPitchTest( -5,   0, synth); // clamped to minimum value
    }

    // New Settings

    public void testEspeakVariant()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_variant", "klatt2-old");
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("klatt2-old"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void espeakRateTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_rate", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(settingValue));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void testEspeakRate()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakRateTest(500, 450, synth); // clamped to maximum value
        espeakRateTest(400, 400, synth);
        espeakRateTest(200, 200, synth);
        espeakRateTest(175, 175, synth); // default value
        espeakRateTest(150, 150, synth);
        espeakRateTest( 70,  80, synth); // clamped to minimum value
    }

    public void espeakPitchTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_pitch", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(settingValue));
    }

    public void testEspeakPitch()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakPitchTest(110, 100, synth); // clamped to maximum value
        espeakPitchTest(100, 100, synth);
        espeakPitchTest( 50,  50, synth); // default value
        espeakPitchTest( 10,  10, synth);
        espeakPitchTest( -5,   0, synth); // clamped to minimum value
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

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("klatt4"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }

    public void testEspeakRateWithDefaultRate()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_rate", Integer.toString(50));
        editor.putString("espeak_rate", Integer.toString(200));
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(200));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
    }
}
