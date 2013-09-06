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

package com.reecedunn.espeak;

import java.util.regex.Pattern;

public class VoiceVariant {
    private static final Pattern mVariantPattern = Pattern.compile("-");

    public static final String MALE = "male";
    public static final String FEMALE = "female";

    public final String variant;
    public final int gender;
    public final int age;

    protected VoiceVariant(String variant, int age) {
        if (variant.equals(MALE)) {
            this.variant = null;
            this.gender = SpeechSynthesis.GENDER_MALE;
        } else if (variant.equals(FEMALE)) {
            this.variant = null;
            this.gender = SpeechSynthesis.GENDER_FEMALE;
        } else {
            this.variant = variant;
            this.gender = SpeechSynthesis.GENDER_UNSPECIFIED;
        }
        this.age = age;
    }

    @Override
    public String toString() {
        final String ret;
        if (gender == SpeechSynthesis.GENDER_MALE) {
            ret = MALE;
        } else if (gender == SpeechSynthesis.GENDER_FEMALE) {
            ret = FEMALE;
        } else {
            ret = variant;
        }
        if (age == SpeechSynthesis.AGE_YOUNG) {
            return ret + "-young";
        } else if (age == SpeechSynthesis.AGE_OLD) {
            return ret + "-old";
        }
        return ret;
    }

    public boolean equals(Object o) {
        if (o instanceof VoiceVariant) {
            VoiceVariant other = (VoiceVariant)o;
            if (variant == null || other.variant == null) {
                return other.variant == null && variant == null && other.gender == gender && other.age == age;
            }
            return other.variant.equals(variant) && other.gender == gender && other.age == age;
        }
        return false;
    }

    public static VoiceVariant parseVoiceVariant(String value) {
        String[] parts = mVariantPattern.split(value);
        int age = SpeechSynthesis.AGE_ANY;
        switch (parts.length) {
        case 1: // variant
            break;
        case 2: // variant-age
            age = parts[1].equals("young") ? SpeechSynthesis.AGE_YOUNG : SpeechSynthesis.AGE_OLD;
            break;
        default:
            return null;
        }
        return new VoiceVariant(parts[0], age);
    }
}