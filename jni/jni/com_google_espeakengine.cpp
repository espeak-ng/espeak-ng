/*
 * Copyright (C) 2008 Google Inc.
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define LOG_TAG "eSpeak Engine"

#include <speak_lib.h>
#include <TtsEngine.h>
#include <Log.h>

/*
 * This is the Manager layer.  It sits on top of the native eSpeak engine
 * and provides the interface to the defined Google TTS engine API.
 * The Google engine API is the boundary to allow a TTS engine to be swapped.
 * The Manager layer also provide the SSML tag interpretation.
 * The supported SSML tags are mapped to corresponding tags natively supported by eSpeak.
 * Native eSpeak functions always begin with espeak_XXX.
 *
 * Only a subset of SSML 1.0 tags are supported.
 * Some SSML tags involve significant complexity.
 * If the language is changed through an SSML tag, there is a latency for the load.
 */

using namespace android;

const char *ESPEAK_DIRECTORY = "espeak-data";

const char *eSpeakBaseResources[] = {"intonations", "phondata", "phonindex", "phontab",
    "en_dict", "voices/en/en-us" };

const int NUM_BASE_RESOURCES = 6;

// Format is {espeak voice, iso3 code, name}
const char *eSpeakSupportedVoices[][3] = {
        {"en-us",  "eng",        "English"},
        {"en-us",  "eng-USA",    "English (US)"},
        {"en",     "eng-GBR",    "English (UK)"},
        {"en-sc",  "eng-GBR-sc", "English (Scottish)"},
        {"en-n",   "eng-GBR-n",  "English (Northern UK)"},
        {"en-rp",  "eng-GBR-rp", "English (Received Pronunciation)"},
        {"en-wm",  "eng-GBR-wm", "English (West Midlands)"},
        {"af",     "afr",        "Afrikaans"},
        {"bs",     "bos",        "Bosnian"},
        {"ca",     "cat",        "Catalan"},
        {"cs",     "ces",        "Czech"},
        {"da",     "dan",        "Danish"},
        {"de",     "deu",        "German"},
        {"el",     "ell",        "Greek"},
        {"eo",     "epo",        "Esperanto"},
        {"es",     "spa",        "Spanish"},
        {"es-la",  "spa-MEX",    "Spanish (Latin America)"},
        {"fi",     "fin",        "Finnish"},
        {"fr",     "fra",        "French"},
        {"hr",     "hrv",        "Croatian"},
        {"hu",     "hun",        "Hungarian"},
        {"it",     "ita",        "Italian"},
        {"kn",     "kan",        "Kannada"},
        {"ku",     "kur",        "Kurdish"},
        {"lv",     "lav",        "Latvian"},
        {"nl",     "nld",        "Dutch"},
        {"pl",     "pol",        "Polish"},
        {"pt",     "por",        "Portuguese (Brazil)"},
        {"pt",     "por-BRA",    "Portuguese (Brazil)"},
        {"pt-pt",  "por-PRT",    "Portuguese"},
        {"ro",     "ron",        "Romanian"},
        {"sk",     "slk",        "Slovak"},
        {"sr",     "srp",        "Serbian"},
        {"sv",     "swe",        "Swedish"},
        {"sw",     "swa",        "Swahili"},
        {"ta",     "tam",        "Tamil"},
        {"tr",     "tur",        "Turkish"},
        {"zh",     "zho",        "Chinese (Mandarin)"},
        {"cy",     "cym",        "Welsh"},
        {"hi",     "hin",        "Hindi"},
        {"hy",     "hye",        "Armenian"},
        {"id",     "ind",        "Indonesian"},
        {"is",     "isl",        "Icelandic"},
        {"ka",     "kat",        "Georgian"},
        {"la",     "lat",        "Latin"},
        {"mk",     "mkd",        "Macedonian"},
        {"no",     "nor",        "Norwegian"},
        {"ru",     "rus",        "Russian"},
        {"sq",     "sqi",        "Albanian"},
        {"vi",     "vie",        "Vietnamese"},
        {"zh-yue", "zho-HKG",    "Chinese (Cantonese)"},
        {"grc",    "grc",        "Ancient Greek"},
        {"jbo",    "jbo",        "Lojban"},
        {"nci",    "nci",        "Nahuatl (Classical)"},
        {"pap",    "pap",        "Papiamento" }
    };

const int NUM_SUPPORTED_VOICES = 55;

/* Integer constants */
const int DEFAULT_SPEECH_RATE = 150;

// Callback to the TTS API
synthDoneCB_t *ttsSynthDoneCBPointer;

char *currentLanguage = (char *) "en-us";
char *currentRate = (char *) "150";
char *eSpeakDataPath = NULL;

char currentLang[10];
char currentCountry[10];
char currentVariant[10];

bool hasInitialized = false;

/* Functions internal to the eSpeak engine wrapper */
static void setSpeechRate(int speechRate) {
  espeak_ERROR err = espeak_SetParameter(espeakRATE, speechRate, 0);
}

/* Functions exposed to the TTS API */

/* Callback from espeak.  Should call back to the TTS API */
static int eSpeakCallback(short *wav, int numsamples, espeak_EVENT *events) {
  LOGI("Callback with %d samples", numsamples);

  int8_t * castedWav = (int8_t *) wav;
  size_t bufferSize = 0;
  if (numsamples < 1) {
    size_t silenceBufferSize = 2;
    int8_t *silence = new int8_t[silenceBufferSize]; // TODO: This will be a small memory leak, but do it this way for now because passing in an empty buffer can cause a crash.
    silence[0] = 0;
    silence[1] = 0;
    ttsSynthDoneCBPointer(events->user_data, 22050, TTS_AUDIO_FORMAT_PCM_16_BIT, 1, silence,
                          silenceBufferSize, TTS_SYNTH_DONE);
    return 1;
  }
  bufferSize = numsamples * sizeof(short);
  ttsSynthDoneCBPointer(events->user_data, 22050, TTS_AUDIO_FORMAT_PCM_16_BIT, 1, castedWav,
                        bufferSize, TTS_SYNTH_PENDING);
  return 0; // continue synthesis (1 is to abort)
}

static bool fileExists(char *fileName) {
  FILE *file = fopen(fileName, "r");

  if (file == NULL) {
    return false;
  } else {
    fclose(file);
    return true;
  }
}

static bool hasBaseResources() {
  char filename[255];

  for (int i = 0; i < NUM_BASE_RESOURCES; i++) {
    sprintf(filename, "%s/%s/%s", eSpeakDataPath, ESPEAK_DIRECTORY, eSpeakBaseResources[i]);

    if (!fileExists(filename)) {
      LOGE("Missing resource: %s", filename);
      return false;
    }
  }

  return true;
}

/* Google Engine API function implementations */

tts_result attemptInit() {
  if (hasInitialized) {
    return TTS_SUCCESS;
  }

  if (!hasBaseResources()) {
    return TTS_FAILURE;
  }

  // TODO Make sure that the speech data is loaded in
  // the directory /sdcard/espeak-data before calling this.
  int sampleRate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 4096, eSpeakDataPath, 0);

  if (sampleRate <= 0) {
    LOGE("eSpeak initialization failed!");
    return TTS_FAILURE;
  }

  espeak_SetSynthCallback(eSpeakCallback);
  espeak_SetParameter(espeakRATE, DEFAULT_SPEECH_RATE, 0);

  espeak_VOICE voice;
  memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
  const char *langNativeString = "en-us"; //Default to US English
  voice.languages = langNativeString;
  voice.variant = 0;
  espeak_SetVoiceByProperties(&voice);

  hasInitialized = true;

  return TTS_SUCCESS;
}

/** init
 *  Allocates eSpeak memory block and initializes the eSpeak system.
 *  synthDoneCBPtr - Pointer to callback function which will receive generated samples
 *  config - the engine configuration parameters, not used here
 *  return tts_result
 */
tts_result TtsEngine::init(synthDoneCB_t synthDoneCBPtr, const char *engineConfig) {
  ttsSynthDoneCBPointer = synthDoneCBPtr;
  hasInitialized = false;

  if ((engineConfig != NULL) && (strlen(engineConfig) > 0)) {
    eSpeakDataPath = (char *) malloc(strlen(engineConfig));
    strcpy(eSpeakDataPath, engineConfig);
  } else {
    eSpeakDataPath = NULL;
    LOGE("Data path not specified!");
    return TTS_FAILURE;
  }

  return attemptInit();
}

/** shutdown
 *  Unloads all eSpeak resources; terminates eSpeak system and frees eSpeak memory block.
 *  return tts_result
 */
tts_result TtsEngine::shutdown(void) {
  if (eSpeakDataPath != NULL) {
    free(eSpeakDataPath);
  }

  espeak_Terminate();

  return TTS_SUCCESS;
}

tts_result TtsEngine::loadLanguage(const char *lang, const char *country, const char *variant) {
  LOGV("loadLanguage(\"%s\", \"%s\", \"%s\")", lang, country, variant);

  return TTS_FAILURE;
}

tts_support_result isLanguageSupported(const char *lang, const char *country, const char *variant,
                                       int *pindex) {
  LOGV("isLanguageSupported(\"%s\", \"%s\", \"%s\")", lang, country, variant);

  if ((lang == NULL) || (strlen(lang) == 0)) {
    LOGE("TtsEngine::isLanguageAvailable called with no language");
    return TTS_LANG_NOT_SUPPORTED;
  }

  if (pindex != NULL) {
    *pindex = -1;
  }

  int langIndex = -1;
  int countryIndex = -1;
  int variantIndex = -1;

  if (strlen(lang) == 3) {
    for (int i = 0; i < NUM_SUPPORTED_VOICES; i++) {
      if (strncmp(lang, eSpeakSupportedVoices[i][1], 3) == 0) {
        LOGI("Found ISO3 language at index %d", i);
        langIndex = i;
        break;
      }
    }
  } else if (strlen(lang) == 2) {
    for (int i = 0; i < NUM_SUPPORTED_VOICES; i++) {
      if (strncmp(lang, eSpeakSupportedVoices[i][0], 2) == 0) {
        LOGI("Found ISO2 language at index %d", i);
        langIndex = i;
        break;
      }
    }
  }

  if (langIndex < 0) {
    LOGV("TtsEngine::isLanguageAvailable called with unsupported language");
    return TTS_LANG_NOT_SUPPORTED;
  }

  if ((country == NULL) || (strlen(country) == 0)) {
    // TODO: Check whether resources are available for this language.

    if (pindex != NULL) {
      *pindex = langIndex;
    }

    LOGI("No country specified, language is available");
    return TTS_LANG_AVAILABLE;
  }

  char lang_country[10];
  sprintf(lang_country, "%s-%s", lang, country);

  // Find country
  if (strlen(country) == 3) {
    for (int i = langIndex; i < NUM_SUPPORTED_VOICES; i++) {
      if ((strcmp(lang_country, eSpeakSupportedVoices[i][1]) == 0)) {
        LOGI("Found ISO3 country at index %d", i);
        countryIndex = i;
        break;
      }
    }
  } else if (strlen(country) == 2) {
    for (int i = langIndex; i < NUM_SUPPORTED_VOICES; i++) {
      if ((strcmp(lang_country, eSpeakSupportedVoices[i][0]) == 0)) {
        LOGI("Found ISO2 country at index %d", i);
        countryIndex = i;
        break;
      }
    }
  }

  if (countryIndex < 0) {
    if (pindex != NULL) {
      *pindex = langIndex;
    }

    LOGI("No country found, language is available");
    return TTS_LANG_AVAILABLE;
  }

  if ((variant == NULL) || (strlen(variant) == 0)) {
    if (pindex != NULL) {
      *pindex = countryIndex;
    }

    LOGI("No variant specified, language and country are available");
    return TTS_LANG_COUNTRY_AVAILABLE;
  }

  char lang_country_variant[15];
  sprintf(lang_country_variant, "%s-%s-%s", lang, country, variant);

  // Find variant
  for (int i = countryIndex; i < NUM_SUPPORTED_VOICES; i++) {
    if ((strcmp(lang_country_variant, eSpeakSupportedVoices[i][1]) == 0)) {
      LOGI("Found variant at index %d", i);
      variantIndex = i;
      break;
    }
  }

  if (variantIndex < 0) {
    if (pindex != NULL) {
      *pindex = countryIndex;
    }

    LOGI("No variant found, language and country are available");
    return TTS_LANG_COUNTRY_AVAILABLE;
  }

  if (pindex != NULL) {
    *pindex = variantIndex;
  }

  LOGI("Language, country, and variant are available");
  return TTS_LANG_COUNTRY_VAR_AVAILABLE;
}

tts_result TtsEngine::setLanguage(const char *lang, const char *country, const char *variant) {
  LOGV("setLanguage(\"%s\", \"%s\", \"%s\")", lang, country, variant);

  // Make sure the engine is initialized!
  attemptInit();

  int index = -1;

  isLanguageSupported(lang, country, variant, &index);

  if (index < 0) {
    LOGE("setLanguage called with unsupported language");
    return TTS_FAILURE;
  }

  strcpy(currentLang, lang);
  strcpy(currentCountry, country);
  strcpy(currentVariant, variant);

  char espeakLangStr[7];
  strcpy(espeakLangStr, eSpeakSupportedVoices[index][0]);

  espeak_VOICE voice;
  memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
  voice.variant = 0;
  voice.languages = espeakLangStr;
  espeak_ERROR err = espeak_SetVoiceByProperties(&voice);
  currentLanguage = new char[strlen(lang)];
  strcpy(currentLanguage, lang);

  if (err != EE_OK) {
    LOGE("Error code %d when setting voice properties!", err);
    return TTS_FAILURE;
  }

  return TTS_SUCCESS;
}

tts_support_result TtsEngine::isLanguageAvailable(const char *lang, const char *country,
                                                  const char *variant) {
  return isLanguageSupported(lang, country, variant, NULL);
}

tts_result TtsEngine::getLanguage(char *language, char *country, char *variant) {
  strcpy(language, currentLang);
  strcpy(country, currentCountry);
  strcpy(variant, currentVariant);

  return TTS_SUCCESS;
}

/** setAudioFormat
 * sets the audio format to use for synthesis, returns what is actually used.
 * @encoding - reference to encoding format
 * @rate - reference to sample rate
 * @channels - reference to number of channels
 * return tts_result
 * */
tts_result TtsEngine::setAudioFormat(tts_audio_format& encoding, uint32_t& rate, int& channels) {
  LOGE("setAudioFormat(%d, %d, %d) is unsupported", encoding, rate, channels);

  // TODO: Fix this!
  return TTS_SUCCESS;
}

// Sets the property with the specified value
tts_result TtsEngine::setProperty(const char *property, const char *value, const size_t size) {
  LOGV("setProperty(\"%s\", \"%s\", %d)", property, value, size);

  /* Set a specific property for the engine.
   Supported properties include: language (locale), rate, pitch, volume.    */
  /* Sanity check */
  if (property == NULL) {
    LOGE("setProperty called with property NULL");
    return TTS_PROPERTY_UNSUPPORTED;
  }

  if (value == NULL) {
    LOGE("setProperty called with value NULL");
    return TTS_VALUE_INVALID;
  }

  espeak_ERROR result;

  if (strncmp(property, "language", 8) == 0) {
    // TODO: Set this property
    result = EE_OK;
  } else if (strncmp(property, "rate", 4) == 0) {
    int rate = atoi(value) * DEFAULT_SPEECH_RATE / 100;
    result = espeak_SetParameter(espeakRATE, rate, 0);
  } else if (strncmp(property, "pitch", 5) == 0) {
    int pitch = atoi(value);
    result = espeak_SetParameter(espeakPITCH, pitch, 0);
  } else if (strncmp(property, "volume", 6) == 0) {
    int volume = atoi(value);
    result = espeak_SetParameter(espeakVOLUME, volume, 0);
  } else {
    return TTS_PROPERTY_UNSUPPORTED;
  }

  if (result == EE_OK) {
    return TTS_SUCCESS;
  } else {
    return TTS_FAILURE;
  }
}

// Sets the property with the specified value
tts_result TtsEngine::getProperty(const char *property, char *value, size_t *iosize) {
  LOGV("getProperty(\"%s\", ...)", property);

  /* Get the property for the engine.
   This property was previously set by setProperty or by default.       */
  /* sanity check */
  if (property == NULL) {
    LOGE("getProperty called with property NULL");
    return TTS_PROPERTY_UNSUPPORTED;
  }

  if (value == NULL) {
    LOGE("getProperty called with value NULL");
    return TTS_VALUE_INVALID;
  }

  if (strncmp(property, "language", 8) == 0) {
    if (currentLanguage == NULL) {
        strcpy(value, "");
    } else {
        if (*iosize < strlen(currentLanguage)+1)  {
            *iosize = strlen(currentLanguage) + 1;
            return TTS_PROPERTY_SIZE_TOO_SMALL;
        }
        strcpy(value, currentLanguage);
    }
    return TTS_SUCCESS;
  } else if (strncmp(property, "rate", 4) == 0) {
    int rate = espeak_GetParameter(espeakRATE, 1) * 100 / DEFAULT_SPEECH_RATE;
    char tmprate[4];
    sprintf(tmprate, "%d", rate);
    if (*iosize < strlen(tmprate)+1) {
        *iosize = strlen(tmprate) + 1;
        return TTS_PROPERTY_SIZE_TOO_SMALL;
    }
    strcpy(value, tmprate);
    return TTS_SUCCESS;
  } else if (strncmp(property, "pitch", 5) == 0) {
    char tmppitch[4];
    sprintf(tmppitch, "%d", espeak_GetParameter(espeakPITCH, 1));
    if (*iosize < strlen(tmppitch)+1) {
        *iosize = strlen(tmppitch) + 1;
        return TTS_PROPERTY_SIZE_TOO_SMALL;
    }
    strcpy(value, tmppitch);
    return TTS_SUCCESS;
  } else if (strncmp(property, "volume", 6) == 0) {
    char tmpvolume[4];
    sprintf(tmpvolume, "%d", espeak_GetParameter(espeakVOLUME, 1));
    if (*iosize < strlen(tmpvolume)+1) {
        *iosize = strlen(tmpvolume) + 1;
        return TTS_PROPERTY_SIZE_TOO_SMALL;
    }
    strcpy(value, tmpvolume);
    return TTS_SUCCESS;
  }

  LOGE("Unsupported property");
  return TTS_PROPERTY_UNSUPPORTED;
}

/** synthesizeText
 *  Synthesizes a text string.
 *  The text string could be annotated with SSML tags.
 *  @text     - text to synthesize
 *  @buffer   - buffer which will receive generated samples
 *  @bufferSize - size of buffer
 *  @userdata - pointer to user data which will be passed back to callback function
 *  return tts_result
 */
tts_result TtsEngine::synthesizeText(const char *text, int8_t *buffer, size_t bufferSize,
                                     void *userdata) {
  LOGI("Synthesize: %s", text);

  espeak_SetSynthCallback(eSpeakCallback);

  unsigned int unique_identifier;

  espeak_Synth(text, strlen(text), 0, // position
                     POS_CHARACTER, 0, // end position (0 means no end position)
                     espeakCHARS_UTF8 | espeakSSML, // use or ignore xml tags
                     &unique_identifier, userdata);
  espeak_Synchronize();

  LOGI("Synthesis done");

  return TTS_SUCCESS;
}

/** stop
 *  Aborts the running synthesis.
 *  return tts_result
 */
tts_result TtsEngine::stop() {
  espeak_Cancel();
  return TTS_SUCCESS;
}

#ifdef __cplusplus
extern "C" {
#endif

TtsEngine* getTtsEngine() {
  return new TtsEngine();
}

#ifdef __cplusplus
}
#endif
