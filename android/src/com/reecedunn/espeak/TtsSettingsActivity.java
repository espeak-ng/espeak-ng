/*
 * Copyright (C) 2013 Reece H. Dunn
 * Copyright (C) 2011 The Android Open Source Project
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

package com.reecedunn.espeak;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;

public class TtsSettingsActivity extends PreferenceActivity {
    @Override
    @SuppressWarnings("deprecation")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Migrate old eyes-free settings to the new settings:

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        final SharedPreferences.Editor editor = prefs.edit();

        String pitch = prefs.getString("espeak_pitch", null);
        if (pitch == null) {
            // Try the old eyes-free setting:
            pitch = prefs.getString("default_pitch", "100");
            int pitchValue = Integer.parseInt(pitch) / 2;
            editor.putString("espeak_pitch", Integer.toString(pitchValue));
        }

        String rate = prefs.getString("espeak_rate", null);
        if (rate == null) {
            // Try the old eyes-free setting:
            SpeechSynthesis engine = new SpeechSynthesis(this, null);
            int defaultValue = engine.Rate.getDefaultValue();
            int maxValue = engine.Rate.getMaxValue();

            rate = prefs.getString("default_rate", "100");
            int rateValue = (Integer.parseInt(rate) / 100) * defaultValue;
            if (rateValue < defaultValue) rateValue = defaultValue;
            if (rateValue > maxValue) rateValue = maxValue;
            editor.putString("espeak_rate", Integer.toString(rateValue));
        }

        String variant = prefs.getString("espeak_variant", null);
        if (variant == null) {
            String gender = prefs.getString("default_gender", "0");
            if (gender.equals("2")) {
                editor.putString("espeak_variant", VoiceVariant.FEMALE);
            } else {
                editor.putString("espeak_variant", VoiceVariant.MALE);
            }
        }

        editor.commit();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
        {
            getFragmentManager().beginTransaction().replace(
                    android.R.id.content,
                    new PrefsEspeakFragment()).commit();
        }
        else
        {
            addPreferencesFromResource(R.xml.preferences);
            createPreferences(TtsSettingsActivity.this, getPreferenceScreen());
        }
    }

    public static class PrefsEspeakFragment extends PreferenceFragment {
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            addPreferencesFromResource(R.xml.preferences);
            createPreferences(getActivity(), getPreferenceScreen());
        }
    }

    private static Preference createSpeakPunctuationPreference(Context context, SpeechSynthesis engine, int titleRes) {
        final String title = context.getString(titleRes);

        final SpeakPunctuationPreference pref = new SpeakPunctuationPreference(context);
        pref.setTitle(title);
        pref.setDialogTitle(title);
        pref.setOnPreferenceChangeListener(mOnPreferenceChanged);
        pref.setPersistent(true);
        pref.setVoiceSettings(new VoiceSettings(PreferenceManager.getDefaultSharedPreferences(context), engine));
        return pref;
    }

    private static Preference createPreference(Context context, SpeechSynthesis.Parameter parameter, String key, int titleRes) {
        final String title = context.getString(titleRes);
        final int defaultValue = parameter.getDefaultValue();

        final SeekBarPreference pref = new SeekBarPreference(context);
        pref.setTitle(title);
        pref.setDialogTitle(title);
        pref.setKey(key);
        pref.setOnPreferenceChangeListener(mOnPreferenceChanged);
        pref.setPersistent(true);

        switch (parameter.getUnitType())
        {
            case Percentage:
                pref.setFormatter(context.getString(R.string.formatter_percentage));
                break;
            case WordsPerMinute:
                pref.setFormatter(context.getString(R.string.formatter_wpm));
                break;
            default:
                throw new IllegalStateException("Unsupported unit type for the parameter.");
        }

        pref.setMin(parameter.getMinValue());
        pref.setMax(parameter.getMaxValue());
        pref.setDefaultValue(defaultValue);

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        final String prefString = prefs.getString(key, null);
        if (prefString == null) {
            pref.setProgress(defaultValue);
        } else {
            pref.setProgress(Integer.parseInt(prefString));
        }

        return pref;
    }

    /**
     * Since the "%s" summary is currently broken, this sets the preference
     * change listener for all {@link ListPreference} views to fill in the
     * summary with the current entry value.
     */
    private static void createPreferences(Context context, PreferenceGroup group) {
        if (group == null) {
            return;
        }

        final int count = group.getPreferenceCount();

        for (int i = 0; i < count; i++) {
            final Preference preference = group.getPreference(i);

            if (preference instanceof PreferenceGroup) {
                createPreferences(null, (PreferenceGroup) preference);
            } else if (preference instanceof ListPreference) {
                preference.setOnPreferenceChangeListener(mOnPreferenceChanged);
            }
        }

        if (context == null) {
            return;
        }

        // Bind eSpeak parameters to preference settings:

        SpeechSynthesis engine = new SpeechSynthesis(context, null);

        group.addPreference(createSpeakPunctuationPreference(context, engine, R.string.espeak_speak_punctuation));
        group.addPreference(createPreference(context, engine.Rate, "espeak_rate", R.string.setting_default_rate));
        group.addPreference(createPreference(context, engine.Pitch, "espeak_pitch", R.string.setting_default_pitch));
        group.addPreference(createPreference(context, engine.PitchRange, "espeak_pitch_range", R.string.espeak_pitch_range));
        group.addPreference(createPreference(context, engine.Volume, "espeak_volume", R.string.espeak_volume));
    }

    private static final OnPreferenceChangeListener mOnPreferenceChanged =
            new OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    if (newValue instanceof String) {
                        String summary = "";
                        if (preference instanceof ListPreference) {
                            final ListPreference listPreference = (ListPreference) preference;
                            final int index = listPreference.findIndexOfValue((String) newValue);
                            final CharSequence[] entries = listPreference.getEntries();

                            if (index >= 0 && index < entries.length) {
                                summary = entries[index].toString();
                            }
                        } else if (preference instanceof SeekBarPreference) {
                            final SeekBarPreference seekBarPreference = (SeekBarPreference) preference;
                            String formatter = seekBarPreference.getFormatter();
                            summary = String.format(formatter, (String)newValue);
                        } else {
                            summary = (String)newValue;
                        }
                        preference.setSummary(summary);
                    }
                    return true;
                }
            };
}
