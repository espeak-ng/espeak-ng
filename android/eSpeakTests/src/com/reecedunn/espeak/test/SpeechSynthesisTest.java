/*
 * Copyright (C) 2012 Reece H. Dunn
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

import java.util.List;
import java.util.Locale;

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.SpeechSynthesis.Voice;

import android.media.AudioFormat;
import android.speech.tts.TextToSpeech;
import android.test.AndroidTestCase;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

public class SpeechSynthesisTest extends AndroidTestCase
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

    private List<Voice> mVoices = null;

    public List<Voice> getVoices()
    {
        if (mVoices == null)
        {
            final SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
            mVoices = synth.getAvailableVoices();
            assertThat(mVoices, is(notNullValue()));
            assertThat(mVoices.size(), is(78));
        }
        return mVoices;
    }

    public Voice getVoice(String name)
    {
        for (Voice voice : getVoices())
        {
            if (voice.name.equals(name))
            {
                return voice;
            }
        }
        return null;
    }

    public void checkVoice(String name, String identifier, String language, String iso3Language, String country, String iso3Country, String variant, int gender)
    {
        final Voice voice = getVoice(name);
        assertThat(voice, is(notNullValue()));
        assertThat(voice.name, is(name));
        assertThat(voice.identifier, is(identifier));
        assertThat(voice.age, is(0));
        assertThat(voice.gender, is(gender));
        assertThat(voice.locale.getLanguage(), is(language));
        assertThat(voice.locale.getISO3Language(), is(iso3Language));
        assertThat(voice.locale.getCountry(), is(country));
        assertThat(voice.locale.getISO3Country(), is(iso3Country));
        assertThat(voice.locale.getVariant(), is(variant));
        assertThat(voice.toString(), is(name));
    }

    public void testConstruction()
    {
        final SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        assertThat(synth.getSampleRate(), is(22050));
        assertThat(synth.getChannelCount(), is(1));
        assertThat(synth.getAudioFormat(), is(AudioFormat.ENCODING_PCM_16BIT));
        assertThat(synth.getBufferSizeInBytes(), is(22050));
    }

    public void testAvailableVoices()
    {
        //        :              :            : language        : country     :            :
        //        : name         : identifier : 639-1 : 639-2/T : 2    : 3    : variant    : gender
        checkVoice("af",          "af",        "af",   "afr",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Afrikaans
        checkVoice("ak",          "test/ak",   "ak",   "aka",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Akan
        checkVoice("am",          "test/am",   "am",   "amh",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Amharic
        checkVoice("az",          "test/az",   "az",   "aze",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Azerbaijani
        checkVoice("bg",          "test/bg",   "bg",   "bul",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Bulgarian
        checkVoice("bs",          "bs",        "bs",   "bos",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Bosnian
        checkVoice("ca",          "ca",        "ca",   "cat",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Catalan
        checkVoice("cs",          "cs",        "cs",   "ces",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Czech
        checkVoice("cy",          "cy",        "cy",   "cym",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Welsh
        checkVoice("da",          "da",        "da",   "dan",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Danish
        checkVoice("de",          "de",        "de",   "deu",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // German
        checkVoice("dv",          "test/dv",   "dv",   "div",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Divehi
        checkVoice("el",          "el",        "el",   "ell",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Greek (Modern)
        checkVoice("en",          "default",   "en",   "eng",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // German
        checkVoice("en-sc",       "en/en-sc",  "en",   "eng",    "GB",  "GBR", "scotland",   SpeechSynthesis.GENDER_MALE); // English (Scotland)
        checkVoice("en-uk",       "en/en",     "en",   "eng",    "GB",  "GBR", "",           SpeechSynthesis.GENDER_MALE); // English (UK)
        checkVoice("en-uk-north", "en/en-n",   "en",   "eng",    "GB",  "GBR", "north",      SpeechSynthesis.GENDER_MALE); // English (Lancashire)
        checkVoice("en-uk-rp",    "en/en-rp",  "en",   "eng",    "GB",  "GBR", "rp",         SpeechSynthesis.GENDER_MALE); // English (Received Pronunciation)
        checkVoice("en-uk-wmids", "en/en-wm",  "en",   "eng",    "GB",  "GBR", "wmids",      SpeechSynthesis.GENDER_MALE); // English (West Midlands)
        checkVoice("en-us",       "en/en-us",  "en",   "eng",    "US",  "USA", "",           SpeechSynthesis.GENDER_MALE); // English (US)
        checkVoice("en-wi",       "en/en-wi",  "en",   "eng",    "029", "",    "",           SpeechSynthesis.GENDER_MALE); // English (Caribbean)
        checkVoice("eo",          "eo",        "eo",   "epo",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Esperanto
        checkVoice("es-la",       "es-la",     "es",   "spa",    "419", "",    "",           SpeechSynthesis.GENDER_MALE); // Spanish (Latin America & Caribbean)
        checkVoice("et",          "et",        "et",   "est",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Estonian
        checkVoice("fi",          "fi",        "fi",   "fin",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Finnish
        checkVoice("fr-fr",       "fr",        "fr",   "fra",    "FR",  "FRA", "",           SpeechSynthesis.GENDER_MALE); // French (France)
        checkVoice("ga",          "test/ga",   "ga",   "gle",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Irish
        checkVoice("grc",         "test/grc",  "grc",  "grc",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Greek (Ancient)
        checkVoice("hi",          "hi",        "hi",   "hin",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Hindi
        checkVoice("hr",          "hr",        "hr",   "hrv",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Croatian
        checkVoice("ht",          "test/ht",   "ht",   "hat",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Haitian Creole
        checkVoice("hu",          "hu",        "hu",   "hun",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Hungarian
        checkVoice("hy",          "hy",        "hy",   "hye",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Armenian
        checkVoice("hy-west",     "hy-west",   "hy",   "hye",    "",    "",    "arevmda",    SpeechSynthesis.GENDER_MALE); // Armenian (Western)
        checkVoice("id",          "id",        "in",   "ind",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Indonesia
        checkVoice("is",          "is",        "is",   "isl",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Icelandic
        checkVoice("it",          "it",        "it",   "ita",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Italian
        checkVoice("jbo",         "test/jbo",  "jbo",  "jbo",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Lojban
        checkVoice("ka",          "ka",        "ka",   "kat",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Georgian
        checkVoice("kk",          "test/kk",   "kk",   "kaz",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Kazakh
        checkVoice("kl",          "test/kl",   "kl",   "kal",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Greenlandic
        checkVoice("kn",          "kn",        "kn",   "kan",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Kannada
        checkVoice("ko",          "test/ko",   "ko",   "kor",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Korean
        checkVoice("ku",          "ku",        "ku",   "kur",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Kurdish
        checkVoice("la",          "la",        "la",   "lat",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Latin
        checkVoice("lt",          "test/lt",   "lt",   "lit",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Lithuanian
        checkVoice("lv",          "lv",        "lv",   "lav",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Latvian
        checkVoice("ml",          "ml",        "ml",   "mal",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Malayalam
        checkVoice("mt",          "test/mt",   "mt",   "mlt",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Maltese
        checkVoice("nci",         "test/nci",  "nci",  "",       "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Classical Nahuatl
        checkVoice("ne",          "test/ne",   "ne",   "nep",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Nepali
        checkVoice("nl",          "nl",        "nl",   "nld",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Dutch
        checkVoice("no",          "no",        "no",   "nor",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Norwegian
        checkVoice("nso",         "test/nso",  "nso",  "nso",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Sotho (Northern)
        checkVoice("pa",          "test/pa",   "pa",   "pan",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Panjabi
        checkVoice("pap",         "test/pap",  "pap",  "pap",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Papiamento
        checkVoice("prs",         "test/prs",  "prs",  "",       "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Dari (Afghan Persian)
        checkVoice("pt-br",       "pt",        "pt",   "por",    "BR",  "BRA", "",           SpeechSynthesis.GENDER_MALE); // Portuguese (Brazil)
        checkVoice("pt-pt",       "pt-pt",     "pt",   "por",    "PT",  "PRT", "",           SpeechSynthesis.GENDER_MALE); // Portuguese (Portugal)
        checkVoice("ro",          "ro",        "ro",   "ron",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Romanian
        checkVoice("ru",          "ru",        "ru",   "rus",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Russian
        checkVoice("rw",          "test/rw",   "rw",   "kin",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Kinyarwanda
        checkVoice("si",          "test/si",   "si",   "sin",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Sinhalese
        checkVoice("sk",          "sk",        "sk",   "slk",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Slovak
        checkVoice("sl",          "test/sl",   "sl",   "slv",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Slovenian
        checkVoice("sq",          "sq",        "sq",   "sqi",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Albanian
        checkVoice("sr",          "sr",        "sr",   "srp",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Serbian
        checkVoice("sv",          "sv",        "sv",   "swe",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Swedish
        checkVoice("ta",          "ta",        "ta",   "tam",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Tamil
        checkVoice("te",          "test/te",   "te",   "tel",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Telugu
        checkVoice("tn",          "test/tn",   "tn",   "tsn",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Setswana
        checkVoice("tr",          "tr",        "tr",   "tur",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Turkish
        checkVoice("tt",          "test/tt",   "tt",   "tat",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Tatar
        checkVoice("ur",          "test/ur",   "ur",   "urd",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Urdu
        checkVoice("vi",          "vi",        "vi",   "vie",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Vietnamese
        checkVoice("wo",          "test/wo",   "wo",   "wol",    "",    "",    "",           SpeechSynthesis.GENDER_UNSPECIFIED); // Wolof
        checkVoice("zh",          "zh",        "zh",   "zho",    "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Chinese (Mandarin)
        checkVoice("zh-yue",      "zh-yue",    "yue",  "",       "",    "",    "",           SpeechSynthesis.GENDER_MALE); // Chinese (Cantonese)
    }

    public void testMatchVoiceWithLanguage()
    {
        final Voice voice = getVoice("de"); // language="de" country="" variant=""
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(fr), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_BE), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_1694acad), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_FR_1694acad), is(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(de), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(de_1996), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(de_DE), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(de_CH_1901), is(TextToSpeech.LANG_AVAILABLE));

        assertThat(voice.match(deu), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(deu_1996), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(deu_DEU), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(deu_CHE_1901), is(TextToSpeech.LANG_AVAILABLE));
    }

    public void testMatchVoiceWithLanguageAndCountry()
    {
        final Voice voice = getVoice("fr-fr"); // language="fr" country="fr" variant=""
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(de), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_1996), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_DE), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_CH_1901), is(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(fr), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_FR), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(fr_BE), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_1694acad), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fr_FR_1694acad), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(fr_BE_1694acad), is(TextToSpeech.LANG_AVAILABLE));

        assertThat(voice.match(fra), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_FRA), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(fra_BEL), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_1694acad), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(fra_FRA_1694acad), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(fra_BEL_1694acad), is(TextToSpeech.LANG_AVAILABLE));
    }

    public void testMatchVoiceWithLanguageAndVariant()
    {
        final Voice voice = getVoice("hy-west"); // language="hy" country="" variant="arevmda"
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(fr), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_BE), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_1694acad), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(fr_FR_1694acad), is(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(hy), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(hy_AM), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(hy_arevela), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(hy_arevmda), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(hy_AM_arevela), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(hy_AM_arevmda), is(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.

        assertThat(voice.match(hye), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(hye_ARM), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(hye_arevela), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(hye_arevmda), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(hye_ARM_arevela), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(hye_ARM_arevmda), is(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.
    }

    public void testMatchVoiceWithLanguageCountryAndVariant()
    {
        final Voice voice = getVoice("en-sc"); // language="en" country="GB" variant="scotland"
        assertThat(voice, is(notNullValue()));

        assertThat(voice.match(de), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_1996), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_DE), is(TextToSpeech.LANG_NOT_SUPPORTED));
        assertThat(voice.match(de_CH_1901), is(TextToSpeech.LANG_NOT_SUPPORTED));

        assertThat(voice.match(en), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(en_GB), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(en_US), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(en_scotland), is(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.
        assertThat(voice.match(en_GB_scotland), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(en_GB_north), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));

        assertThat(voice.match(eng), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(eng_GBR), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
        assertThat(voice.match(eng_USA), is(TextToSpeech.LANG_AVAILABLE));
        assertThat(voice.match(eng_scotland), is(TextToSpeech.LANG_AVAILABLE)); // NOTE: Android does not support LANG_VAR_AVAILABLE.
        assertThat(voice.match(eng_GBR_scotland), is(TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE));
        assertThat(voice.match(eng_GBR_north), is(TextToSpeech.LANG_COUNTRY_AVAILABLE));
    }
}
