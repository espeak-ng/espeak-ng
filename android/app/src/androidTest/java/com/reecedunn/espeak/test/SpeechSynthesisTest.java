/*
 * Copyright (C) 2012-2015 Reece H. Dunn
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

import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.Voice;

import android.media.AudioFormat;
import android.speech.tts.TextToSpeech;
import android.util.Log;

import static com.reecedunn.espeak.test.TtsMatcher.isTtsLangCode;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;
import static org.hamcrest.core.AnyOf.anyOf;

public class SpeechSynthesisTest extends TextToSpeechTestCase
{
    public static final Locale af = new Locale("af"); // Afrikaans
    public static final Locale afr = new Locale("afr"); // Afrikaans

    public static final Locale de = new Locale("de"); // German
    public static final Locale de_DE = new Locale("de", "DE"); // German (Germany)
    public static final Locale de_1996 = new Locale("de", "", "1996"); // German (1996 Orthography)
    public static final Locale de_CH_1901 = new Locale("de", "CH", "1901"); // German (Traditional Orthography,Switzerland)

    public static final Locale deu = new Locale("deu"); // German
    public static final Locale deu_DEU = new Locale("deu", "DEU"); // German (Germany)
    public static final Locale deu_1996 = new Locale("deu", "", "1996"); // German (1996 Orthography)
    public static final Locale deu_CHE_1901 = new Locale("deu", "CHE", "1901"); // German (Traditional Orthography,Switzerland)

    public static final Locale fr = new Locale("fr"); // French
    public static final Locale fr_FR = new Locale("fr", "FR"); // French (France)
    public static final Locale fr_BE = new Locale("fr", "BE"); // French (Belgium)
    public static final Locale fr_1694acad = new Locale("fr", "", "1694acad"); // French (Early Modern French)
    public static final Locale fr_FR_1694acad = new Locale("fr", "FR", "1694acad"); // French (Early Modern French,France)
    public static final Locale fr_BE_1694acad = new Locale("fr", "BE", "1694acad"); // French (Early Modern French,Belgium)

    public static final Locale fra = new Locale("fra"); // French
    public static final Locale fra_FRA = new Locale("fra", "FRA"); // French (France)
    public static final Locale fra_BEL = new Locale("fra", "BEL"); // French (Belgium)
    public static final Locale fra_1694acad = new Locale("fra", "", "1694acad"); // French (Early Modern French)
    public static final Locale fra_FRA_1694acad = new Locale("fra", "FRA", "1694acad"); // French (Early Modern French,France)
    public static final Locale fra_BEL_1694acad = new Locale("fra", "BEL", "1694acad"); // French (Early Modern French,Belgium)

    public static final Locale hy = new Locale("hy"); // Armenian
    public static final Locale hy_AM = new Locale("hy", "AM"); // Armenian (Armenia)
    public static final Locale hy_arevela = new Locale("hy", "", "arevela"); // Armenian (Eastern)
    public static final Locale hy_arevmda = new Locale("hy", "", "arevmda"); // Armenian (Western)
    public static final Locale hy_AM_arevela = new Locale("hy", "AM", "arevela"); // Armenian (Eastern,Armenia)
    public static final Locale hy_AM_arevmda = new Locale("hy", "AM", "arevmda"); // Armenian (Western,Armenia)

    public static final Locale hye = new Locale("hye"); // Armenian
    public static final Locale hye_ARM = new Locale("hye", "ARM"); // Armenian (Armenia)
    public static final Locale hye_arevela = new Locale("hye", "", "arevela"); // Armenian (Eastern)
    public static final Locale hye_arevmda = new Locale("hye", "", "arevmda"); // Armenian (Western)
    public static final Locale hye_ARM_arevela = new Locale("hye", "ARM", "arevela"); // Armenian (Eastern,Armenia)
    public static final Locale hye_ARM_arevmda = new Locale("hye", "ARM", "arevmda"); // Armenian (Western,Armenia)

    public static final Locale en = new Locale("en"); // English
    public static final Locale en_GB = new Locale("en", "GB"); // English (Great Britain)
    public static final Locale en_US = new Locale("en", "US"); // English (USA)
    public static final Locale en_scotland = new Locale("en", "", "scotland"); // English (Scottish)
    public static final Locale en_GB_scotland = new Locale("en", "GB", "scotland"); // English (Scottish,Great Britain)
    public static final Locale en_GB_north = new Locale("en", "GB", "north"); // English (North,Great Britain)

    public static final Locale eng = new Locale("en"); // English
    public static final Locale eng_GBR = new Locale("en", "GBR"); // English (Great Britain)
    public static final Locale eng_USA = new Locale("en", "USA"); // English (USA)
    public static final Locale eng_scotland = new Locale("en", "", "scotland"); // English (Scottish)
    public static final Locale eng_GBR_scotland = new Locale("en", "GBR", "scotland"); // English (Scottish,Great Britain)
    public static final Locale eng_GBR_north = new Locale("en", "GBR", "north"); // English (North,Great Britain)

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

    private Map<String, Voice> mVoices = null;
    private Set<String> mAdded = new HashSet<String>();
    private Set<String> mRemoved = new HashSet<String>();

    public Map<String, Voice> getVoices()
    {
        if (mVoices == null)
        {
            final SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
            mVoices = new HashMap<String, Voice>();
            for (Voice voice : synth.getAvailableVoices()) {
                assertThat(mVoices.get(voice.name), is(nullValue()));
                mVoices.put(voice.name, voice);
            }
            assertThat(mVoices, is(notNullValue()));

            Set<String> voices = new HashSet<String>();
            for (Voice data : mVoices.values())
            {
                voices.add(data.name);
            }

            Set<String> expected = new HashSet<String>();
            for (VoiceData.Voice data : VoiceData.voices)
            {
                expected.add(data.name);
            }

            for (String voice : voices)
            {
                if (!expected.contains(voice))
                {
                    mAdded.add(voice);
                }
            }

            for (String voice : expected)
            {
                if (!voices.contains(voice))
                {
                    mRemoved.add(voice);
                }
            }
        }
        return mVoices;
    }

    public Voice getVoice(String name)
    {
        return getVoices().get(name);
    }

    public void testConstruction()
    {
        final SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        assertThat(synth.getSampleRate(), is(22050));
        assertThat(synth.getChannelCount(), is(1));
        assertThat(synth.getAudioFormat(), is(AudioFormat.ENCODING_PCM_16BIT));
    }

    public void testJavaToIanaLanguageCode()
    {
        for (VoiceData.Voice data : VoiceData.voices)
        {
            assertThat(SpeechSynthesis.getIanaLanguageCode(data.javaLanguage), is(data.ianaLanguage));
        }
    }

    public void testJavaToIanaCountryCode()
    {
        for (VoiceData.Voice data : VoiceData.voices)
        {
            assertThat(SpeechSynthesis.getIanaCountryCode(data.javaCountry), is(data.ianaCountry));
        }
    }

    public void testAddedVoices()
    {
        getVoices(); // Ensure that the voice data has been populated.
        assertThat(mAdded.toString(), is("[]"));
    }

    public void testRemovedVoices()
    {
        getVoices(); // Ensure that the voice data has been populated.
        assertThat(mRemoved.toString(), is("[]"));
    }

    public void testVoiceData()
    {
        for (VoiceData.Voice data : VoiceData.voices)
        {
            if (mRemoved.contains(data.name))
            {
                Log.i("SpeechSynthesisTest", "Skipping the missing voice '" + data.name + "'");
                continue;
            }

            String context = "[voice]";
            try
            {
                final Voice voice = getVoice(data.name);
                assertThat(voice, is(notNullValue()));

                context = "[name]";
                assertThat(voice.name, is(data.name));
                context = "[identifier]";
                assertThat(voice.identifier, is(data.identifier));
                context = "[age]";
                assertThat(voice.age, is(0));
                context = "[gender]";
                assertThat(voice.gender, is(data.gender));
                context = "[locale:language]";
                assertThat(voice.locale.getLanguage(), is(data.ianaLanguage));
                context = "[locale:iso3language]";
                assertThat(voice.locale.getISO3Language(), is(data.javaLanguage));
                context = "[locale:country]";
                assertThat(voice.locale.getCountry(), is(data.ianaCountry));
                context = "[locale:iso3country]";
                assertThat(voice.locale.getISO3Country(), is(data.javaCountry));
                context = "[locale:variant]";
                assertThat(voice.locale.getVariant(), is(data.variant));
                context = "[toString]";
                assertThat(voice.toString(), is(data.locale));
            }
            catch (AssertionError e)
            {
                throw new VoiceData.Exception(data, context, e);
            }
        }
    }

    public void testMatchVoiceWithLanguage()
    {
        final Voice voice = getVoice("de"); // language="de" country="" variant=""
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(fr), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_BE), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_1694acad), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_FR_1694acad), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(de), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(de_1996), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(de_DE), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(de_CH_1901), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));

        assertThat(voice.match(deu), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(deu_1996), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(deu_DEU), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(deu_CHE_1901), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
    }

    public void testMatchVoiceWithLanguageAndCountry()
    {
        final Voice voice = getVoice("fr-be"); // language="fr" country="BE" variant=""
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(de), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_1996), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_DE), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_CH_1901), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(fr), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_FR), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_BE), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(fr_1694acad), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_FR_1694acad), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_BE_1694acad), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));

        assertThat(voice.match(fra), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_FRA), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_BEL), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(fra_1694acad), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_FRA_1694acad), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_BEL_1694acad), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
    }

    public void testMatchVoiceWithLanguageCountryAndVariant()
    {
        final Voice voice = getVoice("en-gb-scotland"); // language="en" country="GB" variant="scotland"
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(de), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_1996), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_DE), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_CH_1901), isTtsLangCode(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(en), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(en_GB), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(en_US), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(en_scotland), isTtsLangCode(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.
        assertThat(voice.match(en_GB_scotland), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(en_GB_north), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));

        assertThat(voice.match(eng), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(eng_GBR), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(eng_USA), isTtsLangCode(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(eng_scotland), isTtsLangCode(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.
        assertThat(voice.match(eng_GBR_scotland), isTtsLangCode(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(eng_GBR_north), isTtsLangCode(TextToSpeech.LANG_COUNTRY_AVAILABLE));
    }

    public void testGetSampleText()
    {
        final String[] currentLocales = getContext().getResources().getAssets().getLocales();
        for (VoiceData.Voice data : VoiceData.voices)
        {
            if (mRemoved.contains(data.name))
            {
                Log.i("SpeechSynthesisTest", "Skipping the missing voice '" + data.name + "'");
                continue;
            }

            String context = null;
            try
            {
                final Locale ianaLocale = new Locale(data.ianaLanguage, data.ianaCountry, data.variant);
                context = "[iana:sample-text]";
                assertThat(SpeechSynthesis.getSampleText(getContext(), ianaLocale), isIn(data.sampleText));
                context = "[iana:resource-locale]";
                assertThat(getContext().getResources().getAssets().getLocales(), is(currentLocales));

                if (!data.javaLanguage.equals(""))
                {
                    final Locale javaLocale = new Locale(data.javaLanguage, data.javaCountry, data.variant);
                    context = "[java:sample-text]";
                    assertThat(SpeechSynthesis.getSampleText(getContext(), javaLocale), isIn(data.sampleText));
                    context = "[java:resource-locale]";
                    assertThat(getContext().getResources().getAssets().getLocales(), is(currentLocales));
                }
            }
            catch (AssertionError e)
            {
                throw new VoiceData.Exception(data, context, e);
            }
        }
    }
}
