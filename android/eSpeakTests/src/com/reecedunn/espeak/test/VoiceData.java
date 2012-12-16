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

import com.reecedunn.espeak.SpeechSynthesis;

public class VoiceData
{
    public static class Voice
    {
        public final String name;
        public final String identifier;
        public final String ianaLanguage;
        public final String javaLanguage;
        public final String ianaCountry;
        public final String javaCountry;
        public final String variant;
        public final int    gender;
        public final String displayName;

        public Voice(String name,
                     String identifier,
                     String ianaLanguage,
                     String javaLanguage,
                     String ianaCountry,
                     String javaCountry,
                     String variant,
                     int    gender,
                     String displayName)
        {
            this.name = name;
            this.identifier = identifier;
            this.ianaLanguage = ianaLanguage;
            this.javaLanguage = javaLanguage;
            this.ianaCountry = ianaCountry;
            this.javaCountry = javaCountry;
            this.variant = variant;
            this.gender = gender;
            this.displayName = displayName;
        }
    }

    public static class Exception extends AssertionError
    {
        private static final long serialVersionUID = 1L;

        public Exception(Voice voice, java.lang.Exception context)
        {
            super("Voice \"" + voice.name + "\" -- " + context);
        }
    }

    public static final Voice[] voices = new Voice[] {
        new Voice("af",          "af",       "af",  "afr", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Afrikaans"),
        new Voice("ak",          "test/ak",  "ak",  "aka", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Akan"),
        new Voice("am",          "test/am",  "am",  "amh", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Amharic"),
        new Voice("az",          "test/az",  "az",  "aze", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Azerbaijani"),
        new Voice("bg",          "test/bg",  "bg",  "bul", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Bulgarian"),
        new Voice("bs",          "bs",       "bs",  "bos", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Bosnian"),
        new Voice("ca",          "ca",       "ca",  "cat", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Catalan"),
        new Voice("cs",          "cs",       "cs",  "ces", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Czech"),
        new Voice("cy",          "cy",       "cy",  "cym", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Welsh"),
        new Voice("da",          "da",       "da",  "dan", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Danish"),
        new Voice("de",          "de",       "de",  "deu", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "German"),
        new Voice("dv",          "test/dv",  "dv",  "div", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Divehi"),
        new Voice("el",          "el",       "el",  "ell", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Greek (Modern)"),
        new Voice("en",          "default",  "en",  "eng", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "German"),
        new Voice("en-sc",       "en/en-sc", "en",  "eng", "GB",  "GBR", "scotland", SpeechSynthesis.GENDER_MALE,        "English (Scotland)"),
        new Voice("en-uk",       "en/en",    "en",  "eng", "GB",  "GBR", "",         SpeechSynthesis.GENDER_MALE,        "English (UK)"),
        new Voice("en-uk-north", "en/en-n",  "en",  "eng", "GB",  "GBR", "north",    SpeechSynthesis.GENDER_MALE,        "English (Lancashire)"),
        new Voice("en-uk-rp",    "en/en-rp", "en",  "eng", "GB",  "GBR", "rp",       SpeechSynthesis.GENDER_MALE,        "English (Received Pronunciation)"),
        new Voice("en-uk-wmids", "en/en-wm", "en",  "eng", "GB",  "GBR", "wmids",    SpeechSynthesis.GENDER_MALE,        "English (West Midlands)"),
        new Voice("en-us",       "en/en-us", "en",  "eng", "US",  "USA", "",         SpeechSynthesis.GENDER_MALE,        "English (US)"),
        new Voice("en-wi",       "en/en-wi", "en",  "eng", "029", "",    "",         SpeechSynthesis.GENDER_MALE,        "English (Caribbean)"),
        new Voice("eo",          "eo",       "eo",  "epo", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Esperanto"),
        new Voice("es-la",       "es-la",    "es",  "spa", "419", "",    "",         SpeechSynthesis.GENDER_MALE,        "Spanish (Latin America & Caribbean)"),
        new Voice("et",          "et",       "et",  "est", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Estonian"),
        new Voice("fi",          "fi",       "fi",  "fin", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Finnish"),
        new Voice("fr-fr",       "fr",       "fr",  "fra", "FR",  "FRA", "",         SpeechSynthesis.GENDER_MALE,        "French (France)"),
        new Voice("ga",          "test/ga",  "ga",  "gle", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Irish"),
        new Voice("grc",         "test/grc", "grc", "grc", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Greek (Ancient)"),
        new Voice("hi",          "hi",       "hi",  "hin", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Hindi"),
        new Voice("hr",          "hr",       "hr",  "hrv", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Croatian"),
        new Voice("ht",          "test/ht",  "ht",  "hat", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Haitian Creole"),
        new Voice("hu",          "hu",       "hu",  "hun", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Hungarian"),
        new Voice("hy",          "hy",       "hy",  "hye", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Armenian"),
        new Voice("hy-west",     "hy-west",  "hy",  "hye", "",    "",    "arevmda",  SpeechSynthesis.GENDER_MALE,        "Armenian (Western)"),
        new Voice("id",          "id",       "in",  "ind", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Indonesia"),
        new Voice("is",          "is",       "is",  "isl", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Icelandic"),
        new Voice("it",          "it",       "it",  "ita", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Italian"),
        new Voice("jbo",         "test/jbo", "jbo", "jbo", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Lojban"),
        new Voice("ka",          "ka",       "ka",  "kat", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Georgian"),
        new Voice("kk",          "test/kk",  "kk",  "kaz", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Kazakh"),
        new Voice("kl",          "test/kl",  "kl",  "kal", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Greenlandic"),
        new Voice("kn",          "kn",       "kn",  "kan", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Kannada"),
        new Voice("ko",          "test/ko",  "ko",  "kor", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Korean"),
        new Voice("ku",          "ku",       "ku",  "kur", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Kurdish"),
        new Voice("la",          "la",       "la",  "lat", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Latin"),
        new Voice("lt",          "test/lt",  "lt",  "lit", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Lithuanian"),
        new Voice("lv",          "lv",       "lv",  "lav", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Latvian"),
        new Voice("ml",          "ml",       "ml",  "mal", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Malayalam"),
        new Voice("mt",          "test/mt",  "mt",  "mlt", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Maltese"),
        new Voice("nci",         "test/nci", "nci", "",    "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Classical Nahuatl"),
        new Voice("ne",          "test/ne",  "ne",  "nep", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Nepali"),
        new Voice("nl",          "nl",       "nl",  "nld", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Dutch"),
        new Voice("no",          "no",       "no",  "nor", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Norwegian"),
        new Voice("nso",         "test/nso", "nso", "nso", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Sotho (Northern)"),
        new Voice("pa",          "test/pa",  "pa",  "pan", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Panjabi"),
        new Voice("pap",         "test/pap", "pap", "pap", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Papiamento"),
        new Voice("prs",         "test/prs", "prs", "",    "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Dari (Afghan Persian)"),
        new Voice("pt-br",       "pt",       "pt",  "por", "BR",  "BRA", "",         SpeechSynthesis.GENDER_MALE,        "Portuguese (Brazil)"),
        new Voice("pt-pt",       "pt-pt",    "pt",  "por", "PT",  "PRT", "",         SpeechSynthesis.GENDER_MALE,        "Portuguese (Portugal)"),
        new Voice("ro",          "ro",       "ro",  "ron", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Romanian"),
        new Voice("ru",          "ru",       "ru",  "rus", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Russian"),
        new Voice("rw",          "test/rw",  "rw",  "kin", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Kinyarwanda"),
        new Voice("si",          "test/si",  "si",  "sin", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Sinhalese"),
        new Voice("sk",          "sk",       "sk",  "slk", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Slovak"),
        new Voice("sl",          "test/sl",  "sl",  "slv", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Slovenian"),
        new Voice("sq",          "sq",       "sq",  "sqi", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Albanian"),
        new Voice("sr",          "sr",       "sr",  "srp", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Serbian"),
        new Voice("sv",          "sv",       "sv",  "swe", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Swedish"),
        new Voice("ta",          "ta",       "ta",  "tam", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Tamil"),
        new Voice("te",          "test/te",  "te",  "tel", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Telugu"),
        new Voice("tn",          "test/tn",  "tn",  "tsn", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Setswana"),
        new Voice("tr",          "tr",       "tr",  "tur", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Turkish"),
        new Voice("tt",          "test/tt",  "tt",  "tat", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Tatar"),
        new Voice("ur",          "test/ur",  "ur",  "urd", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Urdu"),
        new Voice("vi",          "vi",       "vi",  "vie", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Vietnamese"),
        new Voice("wo",          "test/wo",  "wo",  "wol", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Wolof"),
        new Voice("zh",          "zh",       "zh",  "zho", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Chinese (Mandarin)"),
        new Voice("zh-yue",      "zh-yue",   "yue", "",    "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Chinese (Cantonese)"),
    };
}
