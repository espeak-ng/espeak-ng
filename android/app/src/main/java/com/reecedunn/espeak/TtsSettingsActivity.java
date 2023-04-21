/*
 * Copyright (C) 2022 Beka Gozalishvili
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

import com.reecedunn.espeak.preference.ImportVoicePreference;
import com.reecedunn.espeak.preference.SeekBarPreference;
import com.reecedunn.espeak.preference.SpeakPunctuationPreference;
import com.reecedunn.espeak.preference.VoiceVariantPreference;

public class TtsSettingsActivity extends PreferenceActivity {

    private static Context storageContext;

    @Override
    @SuppressWarnings("deprecation")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
        {
            PreferenceManager preferenceManager = getPreferenceManager();
            preferenceManager.setStorageDeviceProtected ();
        }
        // Migrate old eyes-free settings to the new settings:

        storageContext = EspeakApp.getStorageContext();
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(storageContext);
        final SharedPreferences.Editor editor = prefs.edit();

        String pitch = prefs.getString(VoiceSettings.PREF_PITCH, null);
        if (pitch == null) {
            // Try the old eyes-free setting:
            pitch = prefs.getString(VoiceSettings.PREF_DEFAULT_PITCH, "100");
            int pitchValue = Integer.parseInt(pitch) / 2;
            editor.putString(VoiceSettings.PREF_PITCH, Integer.toString(pitchValue));
        }

        String rate = prefs.getString(VoiceSettings.PREF_RATE, null);
        if (rate == null) {
            // Try the old eyes-free setting:
            SpeechSynthesis engine = new SpeechSynthesis(storageContext, null);
            int defaultValue = engine.Rate.getDefaultValue();
            int maxValue = engine.Rate.getMaxValue();

            rate = prefs.getString(VoiceSettings.PREF_DEFAULT_RATE, "100");
            int rateValue = (Integer.parseInt(rate) / 100) * defaultValue;
            if (rateValue < defaultValue) rateValue = defaultValue;
            if (rateValue > maxValue) rateValue = maxValue;
            editor.putString(VoiceSettings.PREF_RATE, Integer.toString(rateValue));
        }

        String variant = prefs.getString(VoiceSettings.PREF_VARIANT, null);
        if (variant == null) {
            String gender = prefs.getString(VoiceSettings.PREF_DEFAULT_GENDER, "0");
            if (gender.equals("2")) {
                editor.putString(VoiceSettings.PREF_VARIANT, VoiceVariant.FEMALE);
            } else {
                editor.putString(VoiceSettings.PREF_VARIANT, VoiceVariant.MALE);
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

    private static Preference createImportVoicePreference(Context context) {
        final String title = context.getString(R.string.import_voice_title);

        final ImportVoicePreference pref = new ImportVoicePreference(context);
        pref.setTitle(title);
        pref.setDialogTitle(title);
        pref.setOnPreferenceChangeListener(mOnPreferenceChanged);
        pref.setDescription(R.string.import_voice_description);
        return pref;
    }

    private static Preference createVoiceVariantPreference(Context context, VoiceSettings settings, int titleRes) {
        final String title = context.getString(titleRes);

        final VoiceVariantPreference pref = new VoiceVariantPreference(context);
        pref.setTitle(title);
        pref.setDialogTitle(title);
        pref.setOnPreferenceChangeListener(mOnPreferenceChanged);
        pref.setPersistent(true);
        pref.setVoiceVariant(settings.getVoiceVariant());
        return pref;
    }

    private static Preference createSpeakPunctuationPreference(Context context, VoiceSettings settings, int titleRes) {
        final String title = context.getString(titleRes);

        final SpeakPunctuationPreference pref = new SpeakPunctuationPreference(context);
        pref.setTitle(title);
        pref.setDialogTitle(title);
        pref.setOnPreferenceChangeListener(mOnPreferenceChanged);
        pref.setPersistent(true);
        pref.setVoiceSettings(settings);
        return pref;
    }

    private static Preference createSeekBarPreference(Context context, SpeechSynthesis.Parameter parameter, String key, int titleRes) {
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

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(storageContext);
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
        SpeechSynthesis engine = new SpeechSynthesis(storageContext, null);
        VoiceSettings settings = new VoiceSettings(PreferenceManager.getDefaultSharedPreferences(storageContext), engine);

        group.addPreference(createImportVoicePreference(context));
        group.addPreference(createVoiceVariantPreference(context, settings, R.string.espeak_variant));
        group.addPreference(createSpeakPunctuationPreference(context, settings, R.string.espeak_speak_punctuation));
        group.addPreference(createSeekBarPreference(context, engine.Rate, VoiceSettings.PREF_RATE, R.string.setting_default_rate));
        group.addPreference(createSeekBarPreference(context, engine.Pitch, VoiceSettings.PREF_PITCH, R.string.setting_default_pitch));
        group.addPreference(createSeekBarPreference(context, engine.PitchRange, VoiceSettings.PREF_PITCH_RANGE, R.string.espeak_pitch_range));
        group.addPreference(createSeekBarPreference(context, engine.Volume, VoiceSettings.PREF_VOLUME, R.string.espeak_volume));
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
