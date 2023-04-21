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

import org.json.JSONException;
import org.json.JSONObject;

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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("female"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testDefaultRate()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        defaultRateTest(300, 449, synth); // clamped to maximum value
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("klatt2-old"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testEspeakRate()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakRateTest(500, 449, synth); // clamped to maximum value
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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

    public void espeakPitchRangeTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_pitch_range", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
        assertThat(settings.getPitchRange(), is(settingValue));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testEspeakPitchRange()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakPitchRangeTest(110, 100, synth); // clamped to maximum value
        espeakPitchRangeTest(100, 100, synth);
        espeakPitchRangeTest( 50,  50, synth); // default value
        espeakPitchRangeTest( 10,  10, synth);
        espeakPitchRangeTest( -5,   0, synth); // clamped to minimum value
    }

    public void espeakVolumeTest(int prefValue, int settingValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_volume", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(settingValue));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(settingValue));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testEspeakVolume()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakVolumeTest(210, 200, synth); // clamped to maximum value
        espeakVolumeTest(150, 150, synth);
        espeakVolumeTest(100, 100, synth); // default value
        espeakVolumeTest( 50,  50, synth);
        espeakVolumeTest( -5,   0, synth); // clamped to minimum value
    }

    public void espeakPunctuationLevelTest(int prefValue, int settingValue, String jsonValue, SpeechSynthesis synth)
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_punctuation_level", Integer.toString(prefValue));
        editor.commit();

        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(settingValue));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(jsonValue));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testEspeakPunctuationLevel()
    {
        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        espeakPunctuationLevelTest( 3, SpeechSynthesis.PUNCT_SOME, "some", synth); // clamped to maximum value
        espeakPunctuationLevelTest( 2, SpeechSynthesis.PUNCT_SOME, "some", synth);
        espeakPunctuationLevelTest( 1, SpeechSynthesis.PUNCT_ALL,  "all",  synth);
        espeakPunctuationLevelTest( 0, SpeechSynthesis.PUNCT_NONE, "none", synth);
        espeakPunctuationLevelTest(-1, SpeechSynthesis.PUNCT_NONE, "none", synth); // clamped to minimum value
    }

    public void testEspeakPunctuationCharacters()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("espeak_punctuation_characters", ".?!");
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(synth.Pitch.getDefaultValue()));
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(".?!"));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(".?!"));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("klatt4"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
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
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(200));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(synth.Pitch.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }

    public void testEspeakPitchWithDefaultPitch()
    {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putString("default_pitch", Integer.toString(50));
        editor.putString("espeak_pitch", Integer.toString(75));
        editor.commit();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        VoiceSettings settings = new VoiceSettings(prefs, synth);
        assertThat(settings.getVoiceVariant().toString(), is("male"));
        assertThat(settings.getRate(), is(synth.Rate.getDefaultValue()));
        assertThat(settings.getPitch(), is(75));
        assertThat(settings.getPitchRange(), is(synth.PitchRange.getDefaultValue()));
        assertThat(settings.getVolume(), is(synth.Volume.getDefaultValue()));
        assertThat(settings.getPunctuationLevel(), is(SpeechSynthesis.PUNCT_NONE));
        assertThat(settings.getPunctuationCharacters(), is(nullValue()));

        try {
            JSONObject json = settings.toJSON();
            assertThat(json.opt(VoiceSettings.PRESET_VARIANT), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_VARIANT), is("male"));
            assertThat(json.opt(VoiceSettings.PRESET_RATE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_RATE), is(synth.Rate.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH), is(75));
            assertThat(json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_PITCH_RANGE), is(synth.PitchRange.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_VOLUME), is(instanceOf(Integer.class)));
            assertThat((Integer)json.opt(VoiceSettings.PRESET_VOLUME), is(synth.Volume.getDefaultValue()));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is(instanceOf(String.class)));
            assertThat((String)json.opt(VoiceSettings.PRESET_PUNCTUATION_LEVEL), is("none"));
            assertThat(json.opt(VoiceSettings.PRESET_PUNCTUATION_CHARACTERS), is(nullValue()));
        } catch (JSONException e) {
            assertThat(e.toString(), is(nullValue())); // This will be false; used to report exception.
        }
    }
}
