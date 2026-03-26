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

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.VoiceVariant;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

@RunWith(AndroidJUnit4.class)
public class VoiceVariantTest
{
    @Test
    public void testMaleVoiceVariant()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("male");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_MALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_ANY));
        assertThat(variant.toString(), is("male"));
    }

    @Test
    public void testFemaleVoiceVariant()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("female");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_FEMALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_ANY));
        assertThat(variant.toString(), is("female"));
    }

    @Test
    public void testNamedVoiceVariant()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("klatt1");
        assertThat(variant.variant, is("klatt1"));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_UNSPECIFIED));
        assertThat(variant.age, is(SpeechSynthesis.AGE_ANY));
        assertThat(variant.toString(), is("klatt1"));
    }

    @Test
    public void testMaleVoiceVariantYoung()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("male-young");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_MALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_YOUNG));
        assertThat(variant.toString(), is("male-young"));
    }

    @Test
    public void testFemaleVoiceVariantYoung()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("female-young");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_FEMALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_YOUNG));
        assertThat(variant.toString(), is("female-young"));
    }

    @Test
    public void testNamedVoiceVariantYoung()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("klatt2-young");
        assertThat(variant.variant, is("klatt2"));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_UNSPECIFIED));
        assertThat(variant.age, is(SpeechSynthesis.AGE_YOUNG));
        assertThat(variant.toString(), is("klatt2-young"));
    }

    @Test
    public void testMaleVoiceVariantOld()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("male-old");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_MALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_OLD));
        assertThat(variant.toString(), is("male-old"));
    }

    @Test
    public void testFemaleVoiceVariantOld()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("female-old");
        assertThat(variant.variant, is(nullValue()));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_FEMALE));
        assertThat(variant.age, is(SpeechSynthesis.AGE_OLD));
        assertThat(variant.toString(), is("female-old"));
    }

    @Test
    public void testNamedVoiceVariantOld()
    {
        VoiceVariant variant = VoiceVariant.parseVoiceVariant("klatt3-old");
        assertThat(variant.variant, is("klatt3"));
        assertThat(variant.gender, is(SpeechSynthesis.GENDER_UNSPECIFIED));
        assertThat(variant.age, is(SpeechSynthesis.AGE_OLD));
        assertThat(variant.toString(), is("klatt3-old"));
    }
}
