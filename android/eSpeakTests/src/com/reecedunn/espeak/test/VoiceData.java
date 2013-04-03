/*
 * Copyright (C) 2012-2013 Reece H. Dunn
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
        public final String locale;
        public final String sampleText;

        public Voice(String name,
                     String identifier,
                     String ianaLanguage,
                     String javaLanguage,
                     String ianaCountry,
                     String javaCountry,
                     String variant,
                     int    gender,
                     String displayName,
                     String locale)
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
            this.locale = locale;
            this.sampleText = displayName;
        }

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
            this.locale = ianaLanguage;
            this.sampleText = displayName;
        }

        public Voice(String name,
                     String identifier,
                     String ianaLanguage,
                     String javaLanguage,
                     String ianaCountry,
                     String javaCountry,
                     String variant,
                     int    gender,
                     String displayName,
                     String locale,
                     String sampleText)
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
            this.locale = locale;
            this.sampleText = sampleText;
        }
    }

    public static class Exception extends AssertionError
    {
        private static final long serialVersionUID = 1L;

        public Exception(Voice voice, String context, AssertionError error)
        {
            super("Voice \"" + voice.name + "\" " + context + " -- " + error);
        }
    }

    public static final Voice[] voices = new Voice[] {
        new Voice("af",          "other/af",     "af",  "afr", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Afrikaans",                        "af",             "Hierdie is 'n voorbeeld van gesproke teks in Afrikaans"),
        new Voice("am",          "test/am",      "am",  "amh", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Amharic",                          "am",             "ይሄ በአማርኛ ላይ የተነገረ ጽሑፍ ናሙና ነው።"),
        new Voice("an",          "europe/an",    "an",  "arg", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Aragonese",                        "an",             "This is a sample of text spoken in Aragonese"),
        new Voice("bg",          "europe/bg",    "bg",  "bul", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Bulgarian",                        "bg",             "Това е откъс от изговорен текст на български"),
        new Voice("bn",          "test/bn",      "bn",  "ben", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Bengali",                          "bn",             "This is a sample of text spoken in Bengali"),
        new Voice("bs",          "europe/bs",    "bs",  "bos", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Bosnian",                          "bs",             "This is a sample of text spoken in Bosnian"),
        new Voice("ca",          "europe/ca",    "ca",  "cat", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Catalan",                          "ca",             "Aquesta és una mostra de text dit en veu alta en català"),
        new Voice("cs",          "europe/cs",    "cs",  "ces", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Czech",                            "cs",             "Toto je ukázkový text namluvený v jazyce čeština"),
        new Voice("cy",          "europe/cy",    "cy",  "cym", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Welsh",                            "cy",             "This is a sample of text spoken in Welsh"),
        new Voice("da",          "europe/da",    "da",  "dan", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Danish",                           "da",             "Dette er et eksempel på talt tekst på dansk"),
        new Voice("de",          "de",           "de",  "deu", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "German",                           "de",             "Dies ist ein Beispieltext auf Deutsch."),
        new Voice("el",          "europe/el",    "el",  "ell", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Greek (Modern)",                   "el",             "Αυτό είναι ένα δείγμα κειμένου που εκφέρεται στα Ελληνικά"),
        new Voice("en",          "default",      "en",  "eng", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "English",                          "en",             "This is a sample of text spoken in English"),
        new Voice("en-sc",       "other/en-sc",  "en",  "eng", "GB",  "GBR", "scotland", SpeechSynthesis.GENDER_MALE,        "English (Scotland)",               "en-GB-scotland", "This is a sample of text spoken in English (United Kingdom,Scottish Standard English)"),
        new Voice("en-gb",       "en",           "en",  "eng", "GB",  "GBR", "",         SpeechSynthesis.GENDER_MALE,        "English (UK)",                     "en-GB",          "This is a sample of text spoken in English (United Kingdom)"),
        new Voice("en-uk-north", "other/en-n",   "en",  "eng", "GB",  "GBR", "north",    SpeechSynthesis.GENDER_MALE,        "English (Lancashire)",             "en-GB-north",    "This is a sample of text spoken in English (United Kingdom,NORTH)"),
        new Voice("en-uk-rp",    "other/en-rp",  "en",  "eng", "GB",  "GBR", "rp",       SpeechSynthesis.GENDER_MALE,        "English (Received Pronunciation)", "en-GB-rp",       "This is a sample of text spoken in English (United Kingdom,RP)"),
        new Voice("en-uk-wmids", "other/en-wm",  "en",  "eng", "GB",  "GBR", "wmids",    SpeechSynthesis.GENDER_MALE,        "English (West Midlands)",          "en-GB-wmids",    "This is a sample of text spoken in English (United Kingdom,WMIDS)"),
        new Voice("en-us",       "en-us",        "en",  "eng", "US",  "USA", "",         SpeechSynthesis.GENDER_MALE,        "English (US)",                     "en-US",          "This is a sample of text spoken in English (United States)"),
        new Voice("en-wi",       "other/en-wi",  "en",  "eng", "029", "",    "",         SpeechSynthesis.GENDER_MALE,        "English (Caribbean)",              "en-029",         "This is a sample of text spoken in English"),
        new Voice("eo",          "other/eo",     "eo",  "epo", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Esperanto",                        "eo",             "This is a sample of text spoken in Esperanto"),
        new Voice("es",          "europe/es",    "es",  "spa", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Spanish",                          "es",             "Esto es un ejemplo de texto hablado en español."),
        new Voice("es-la",       "es-la",        "es",  "spa", "419", "",    "",         SpeechSynthesis.GENDER_MALE,        "Spanish (Latin America)",          "es-419",         "Esto es un ejemplo de texto hablado en español."),
        new Voice("et",          "europe/et",    "et",  "est", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Estonian",                         "et",             "This is a sample of text spoken in eesti"),
        new Voice("fa",          "asia/fa",      "fa",  "fas", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Farsi (Persian)",                  "fa",             "این یک نمونه از نوشتار گفته شده در فارسی است"),
        new Voice("fi",          "europe/fi",    "fi",  "fin", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Finnish",                          "fi",             "Tämä on näyte kielellä suomi puhutusta tekstistä"),
        new Voice("fr-be",       "europe/fr-be", "fr",  "fra", "BE",  "BEL", "",         SpeechSynthesis.GENDER_MALE,        "French (Belgium)",                 "fr-BE",          "Voici un exemple de texte énoncé en français (Belgique)."),
        new Voice("fr-fr",       "fr",           "fr",  "fra", "FR",  "FRA", "",         SpeechSynthesis.GENDER_MALE,        "French (France)",                  "fr-FR",          "Voici un exemple de texte énoncé en français (France)."),
        new Voice("ga",          "europe/ga",    "ga",  "gle", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Irish",                            "ga",             "This is a sample of text spoken in Irish"),
        new Voice("grc",         "other/grc",    "grc", "grc", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Greek (Ancient)",                  "grc",            "This is a sample of text spoken in Ancient Greek"),
        new Voice("hi",          "asia/hi",      "hi",  "hin", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Hindi",                            "hi",             "यह हिन्दी में बोले गए पाठ का नमूना है"),
        new Voice("hr",          "europe/hr",    "hr",  "hrv", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Croatian",                         "hr",             "Ovo je primjer teksta izgovorenog na hrvatski"),
        new Voice("hu",          "europe/hu",    "hu",  "hun", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Hungarian",                        "hu",             "Ez egy magyar nyelven felolvasott szöveg mintája."),
        new Voice("hy",          "asia/hy",      "hy",  "hye", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Armenian",                         "hy",             "This is a sample of text spoken in Armenian"),
        new Voice("hy-west",     "asia/hy-west", "hy",  "hye", "",    "",    "arevmda",  SpeechSynthesis.GENDER_MALE,        "Armenian (Western)",               "hy--arevmda",    "This is a sample of text spoken in Armenian (Western Armenian)"),
        new Voice("id",          "asia/id",      "in",  "ind", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Indonesia",                        "in",             "Ini adalah contoh teks yang diucapkan di Bahasa Indonesia"), // NOTE: 'id' is the correct ISO 639-1 code, but Android/Java uses 'in'.
        new Voice("is",          "europe/is",    "is",  "isl", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Icelandic",                        "is",             "This is a sample of text spoken in Icelandic"),
        new Voice("it",          "europe/it",    "it",  "ita", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Italian",                          "it",             "Questo è un esempio di testo parlato in italiano"),
        new Voice("jbo",         "other/jbo",    "jbo", "jbo", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Lojban",                           "jbo",            "This is a sample of text spoken in Lojban"),
        new Voice("ka",          "asia/ka",      "ka",  "kat", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Georgian",                         "ka",             "This is a sample of text spoken in Georgian"),
        new Voice("kl",          "test/kl",      "kl",  "kal", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Greenlandic",                      "kl",             "This is a sample of text spoken in Kalaallisut"),
        new Voice("kn",          "asia/kn",      "kn",  "kan", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Kannada",                          "kn",             "This is a sample of text spoken in Kannada"),
        new Voice("ko",          "test/ko",      "ko",  "kor", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Korean",                           "ko",             "한국어로 읽은 텍스트 샘플입니다."),
        new Voice("ku",          "asia/ku",      "ku",  "kur", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Kurdish",                          "ku",             "This is a sample of text spoken in Kurdish"),
        new Voice("la",          "other/la",     "la",  "lat", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Latin",                            "la",             "This is a sample of text spoken in Latin"),
        new Voice("lt",          "europe/lt",    "lt",  "lit", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Lithuanian",                       "lt",             "Tai teksto, sakomo lietuvių, pavyzdys"),
        new Voice("lv",          "europe/lv",    "lv",  "lav", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Latvian",                          "lv",             "Šis ir izrunāta teksta paraugs šādā valodā: latviešu."),
        new Voice("mk",          "europe/mk",    "mk",  "mkd", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Macedonian",                       "mk",             "This is a sample of text spoken in Macedonian"),
        new Voice("ml",          "asia/ml",      "ml",  "mal", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Malayalam",                        "ml",             "This is a sample of text spoken in Malayalam"),
        new Voice("ms",          "asia/ms",      "ms",  "msa", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Malayalam",                        "ms",             "Ini adalah sampel teks yang dilafazkan dalam Bahasa Melayu"),
        new Voice("ne",          "asia/ne",      "ne",  "nep", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Nepali",                           "ne",             "This is a sample of text spoken in Nepali"),
        new Voice("nl",          "europe/nl",    "nl",  "nld", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Dutch",                            "nl",             "Dit is een voorbeeld van tekst die is uitgesproken in het Nederlands"),
        new Voice("no",          "europe/no",    "no",  "nor", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Norwegian",                        "no",             "This is a sample of text spoken in Norwegian"),
        new Voice("pa",          "asia/pa",      "pa",  "pan", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Panjabi",                          "pa",             "This is a sample of text spoken in Punjabi"),
        new Voice("pap",         "test/pap",     "pap", "pap", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Papiamento",                       "pap",            "This is a sample of text spoken in Papiamento"),
        new Voice("pl",          "europe/pl",    "pl",  "pol", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Polish",                           "pl",             "To jest przykład tekstu mówionego przy ustawieniu polski"),
        new Voice("pt-br",       "pt",           "pt",  "por", "BR",  "BRA", "",         SpeechSynthesis.GENDER_MALE,        "Portuguese (Brazil)",              "pt-BR",          "Este é um exemplo de texto falado em português (Brasil)"),
        new Voice("pt-pt",       "europe/pt-pt", "pt",  "por", "PT",  "PRT", "",         SpeechSynthesis.GENDER_MALE,        "Portuguese (Portugal)",            "pt-PT",          "Este é um exemplo de texto falado em português (Portugal)"),
        new Voice("ro",          "europe/ro",    "ro",  "ron", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Romanian",                         "ro",             "Aceasta este o mostră de text vorbit în română"),
        new Voice("ru",          "europe/ru",    "ru",  "rus", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Russian",                          "ru",             "Так синтезатор речи озвучивает русский текст"),
        new Voice("si",          "test/si",      "si",  "sin", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Sinhalese",                        "si",             "This is a sample of text spoken in Sinhala"),
        new Voice("sk",          "europe/sk",    "sk",  "slk", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Slovak",                           "sk",             "Toto je ukážkový text nahovorený v jazyku slovenčina"),
        new Voice("sl",          "test/sl",      "sl",  "slv", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Slovenian",                        "sl",             "To je vzorec besedila, izgovorjen v slovenščina"),
        new Voice("sq",          "europe/sq",    "sq",  "sqi", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Albanian",                         "sq",             "This is a sample of text spoken in Albanian"),
        new Voice("sr",          "europe/sr",    "sr",  "srp", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Serbian",                          "sr",             "Ово је пример текста који је изговорен на језику Српски"),
        new Voice("sv",          "europe/sv",    "sv",  "swe", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Swedish",                          "sv",             "Detta är ett textexempel som läses på svenska"),
        new Voice("sw",          "other/sw",     "sw",  "swa", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Swahili",                          "sw",             "Hii ni sampuli ya maandishi yaliyonenwa katika Kiswahili"),
        new Voice("ta",          "asia/ta",      "ta",  "tam", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Tamil",                            "ta",             "This is a sample of text spoken in Tamil"),
        new Voice("te",          "test/te",      "te",  "tel", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Telugu",                           "te",             "This is a sample of text spoken in Telugu"),
        new Voice("tr",          "asia/tr",      "tr",  "tur", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Turkish",                          "tr",             "Bu, Türkçe dilinde seslendirilen örnek bir metindir"),
        new Voice("ur",          "test/ur",      "ur",  "urd", "",    "",    "",         SpeechSynthesis.GENDER_UNSPECIFIED, "Urdu",                             "ur",             "This is a sample of text spoken in Urdu"),
        new Voice("vi",          "asia/vi",      "vi",  "vie", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Vietnamese",                       "vi",             "Đây là mẫu văn bản được đọc bằng Tiếng Việt"),
        new Voice("vi-hue",      "asia/vi-hue",  "vi",  "vie", "",    "",    "hue",      SpeechSynthesis.GENDER_MALE,        "Vietnamese",                       "vi--hue",        "Đây là mẫu văn bản được đọc bằng Tiếng Việt (HUE)"),
        new Voice("zh",          "asia/zh",      "zh",  "zho", "",    "",    "",         SpeechSynthesis.GENDER_MALE,        "Chinese (Mandarin)",               "zh",             "This is a sample of text spoken in 中文"),
        new Voice("zh-yue",      "asia/zh-yue",  "zh",  "zho", "HK",  "HKG", "",         SpeechSynthesis.GENDER_MALE,        "Chinese (Cantonese)",              "zh-HK",          "This is a sample of text spoken in 中文 (中華人民共和國香港特別行政區)"),
    };
}
