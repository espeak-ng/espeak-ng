/*
 * Copyright (C) 2025
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

import android.content.SharedPreferences;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public final class LanguageSettings {
    public static final String PREF_SUPPORTED_LANGUAGES = "espeak_supported_languages";

    private LanguageSettings() {
    }

    public static Set<String> getSelectedLanguages(SharedPreferences preferences) {
        if (!preferences.contains(PREF_SUPPORTED_LANGUAGES)) {
            return null; // treat as "all"
        }
        final Set<String> selected = preferences.getStringSet(PREF_SUPPORTED_LANGUAGES, null);
        if (selected == null || selected.isEmpty()) {
            return null; // missing or empty means all
        }
        return new HashSet<String>(selected);
    }

    public static boolean isVoiceEnabled(Voice voice, SharedPreferences preferences) {
        final Set<String> selected = getSelectedLanguages(preferences);
        return selected == null || selected.contains(voice.toString());
    }

    public static List<Voice> filterVoices(List<Voice> voices, SharedPreferences preferences) {
        final Set<String> selected = getSelectedLanguages(preferences);
        if (selected == null) {
            return voices;
        }

        final List<Voice> filtered = new ArrayList<Voice>();
        for (Voice voice : voices) {
            if (selected.contains(voice.toString())) {
                filtered.add(voice);
            }
        }
        return filtered;
    }
}
