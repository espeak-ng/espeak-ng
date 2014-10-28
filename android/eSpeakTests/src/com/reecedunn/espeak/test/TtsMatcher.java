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

import org.hamcrest.Description;
import org.hamcrest.Matcher;
import org.hamcrest.TypeSafeMatcher;

import android.speech.tts.TextToSpeech;

public class TtsMatcher
{
    private static String ttsLangCode(final Integer value) {
        if (value == null) return "null";
        switch (value.intValue()) {
            case TextToSpeech.LANG_AVAILABLE:             return "TextToSpeech.LANG_AVAILABLE";
            case TextToSpeech.LANG_COUNTRY_AVAILABLE:     return "TextToSpeech.LANG_COUNTRY_AVAILABLE";
            case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE: return "TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE";
            case TextToSpeech.LANG_MISSING_DATA:          return "TextToSpeech.LANG_MISSING_DATA";
            case TextToSpeech.LANG_NOT_SUPPORTED:         return "TextToSpeech.LANG_NOT_SUPPORTED";
        }
        return value.toString();
    }

    public static Matcher<Integer> isTtsLangCode(final Integer value) {
        return new TypeSafeMatcher<Integer>() {
            @Override
            protected boolean matchesSafely(Integer item) {
                return item.equals(value);
            }

            @Override
            public void describeTo(Description description) {
                description.appendText("expected constant: ");
                description.appendValue(ttsLangCode(value));
            }

            @Override
            public void describeMismatchSafely(final Integer item, final Description description) {
                description.appendText("was ");
                description.appendValue(ttsLangCode(item));
            }
        };
    }
}
