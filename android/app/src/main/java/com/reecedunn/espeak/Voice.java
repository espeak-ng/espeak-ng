/*
 * Copyright (C) 2012-2013 Reece H. Dunn
 * Copyright (C) 2011 Google Inc.
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

import java.util.Locale;
import java.util.MissingResourceException;

import android.speech.tts.TextToSpeech;

public class Voice {
    public final String name;
    public final String identifier;
    public final int gender;
    public final int age;
    public final Locale locale;

    public Voice(String name, String identifier, int gender, int age, Locale locale) {
        this.name = name;
        this.identifier = identifier;
        this.gender = gender;
        this.age = age;
        this.locale = locale;
    }

    /**
     * Attempts a partial match against a query locale.
     *
     * @param query The locale to match.
     * @return A text-to-speech availability code. One of:
     *         <ul>
     *         <li>{@link TextToSpeech#LANG_NOT_SUPPORTED}
     *         <li>{@link TextToSpeech#LANG_AVAILABLE}
     *         <li>{@link TextToSpeech#LANG_COUNTRY_AVAILABLE}
     *         <li>{@link TextToSpeech#LANG_COUNTRY_VAR_AVAILABLE}
     *         </ul>
     */
    public int match(Locale query) {
        if (!locale.getISO3Language().equals(query.getISO3Language())) {
            return TextToSpeech.LANG_NOT_SUPPORTED;
        }
        try {
            if (!locale.getISO3Country().equals(query.getISO3Country())) {
                return TextToSpeech.LANG_AVAILABLE;
            }
        } catch (MissingResourceException e) {
            return TextToSpeech.LANG_AVAILABLE;
        }
        if (!locale.getVariant().equals(query.getVariant())) {
            return TextToSpeech.LANG_COUNTRY_AVAILABLE;
        }
        return TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE;
    }

    @Override
    public String toString() {
        String ret = locale.getISO3Language();
        if (locale.getISO3Country() != null && !locale.getISO3Country().isEmpty()) {
            ret += '-';
            ret += locale.getISO3Country();
        }
        if (locale.getVariant() != null && !locale.getVariant().isEmpty()) {
            ret += '-';
            ret += locale.getVariant();
        }
        return ret;
    }
}