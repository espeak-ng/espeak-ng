/*
 * Copyright (C) 2012-2017 Reece H. Dunn
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

/*
 * This file contains the JNI bindings to eSpeak used by SpeechSynthesis.java.
 *
 * Android Version: 4.0 (Ice Cream Sandwich)
 * API Version:     14
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <jni.h>

#include <espeak-ng/speak_lib.h>
#include <Log.h>

#define BUFFER_SIZE_IN_MILLISECONDS 300

/* These are helpers for converting a jstring to wchar_t*.
 *
 * This assumes that wchar_t is a 32-bit (UTF-32) value.
 */
//@{

static const char *utf8_read(const char *in, wchar_t *c)
{
	if (((uint8_t)*in) < 0x80)
		*c = *in++;
	else switch (((uint8_t)*in) & 0xF0)
	{
	default:
		*c = ((uint8_t)*in++) & 0x1F;
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		break;
	case 0xE0:
		*c = ((uint8_t)*in++) & 0x0F;
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		break;
	case 0xF0:
		*c = ((uint8_t)*in++) & 0x07;
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		*c = (*c << 6) + (((uint8_t)*in++) & 0x3F);
		break;
	}
	return in;
}

static wchar_t *unicode_string(JNIEnv *env, jstring str)
{
  if (str == NULL) return NULL;

  const char *utf8 = (*env)->GetStringUTFChars(env, str, NULL);
  wchar_t *utf32 = (wchar_t *)malloc((strlen(utf8) + 1) * sizeof(wchar_t));

  const char *utf8_current = utf8;
  wchar_t *utf32_current = utf32;
  while (*utf8_current)
  {
    utf8_current = utf8_read(utf8_current, utf32_current);
    ++utf32_current;
  }
  *utf32_current = 0;

  (*env)->ReleaseStringUTFChars(env, str, utf8);
  return utf32;
}

//@}

#define LOG_TAG "eSpeakService"
#define DEBUG true

enum synthesis_result {
  SYNTH_CONTINUE = 0,
  SYNTH_ABORT = 1
};

static JavaVM *jvm = NULL;
jmethodID METHOD_nativeSynthCallback;

static JNIEnv *getJniEnv() {
  JNIEnv *env = NULL;
  (*jvm)->AttachCurrentThread(jvm, &env, NULL);
  return env;
}

/* Callback from espeak.  Should call back to the TTS API */
static int SynthCallback(short *audioData, int numSamples,
                         espeak_EVENT *events) {
  JNIEnv *env = getJniEnv();
  jobject object = (jobject)events->user_data;

  if (numSamples < 1) {
    (*env)->CallVoidMethod(env, object, METHOD_nativeSynthCallback, NULL);
    return SYNTH_ABORT;
  } else {
    jbyteArray arrayAudioData = (*env)->NewByteArray(env, numSamples * 2);
    (*env)->SetByteArrayRegion(env, arrayAudioData, 0, (numSamples * 2), (jbyte *) audioData);
    (*env)->CallVoidMethod(env, object, METHOD_nativeSynthCallback, arrayAudioData);
    return SYNTH_CONTINUE;
  }
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

JNIEXPORT jint
JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  jvm = vm;
  JNIEnv *env;

  if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    LOGE("Failed to get the environment using GetEnv()");
    return -1;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeClassInit(
    JNIEnv* env, jclass clazz) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  METHOD_nativeSynthCallback = (*env)->GetMethodID(env, clazz, "nativeSynthCallback", "([B)V");

  return JNI_TRUE;
}

JNIEXPORT jint
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeCreate(
    JNIEnv *env, jobject object, jstring path) {
  if (DEBUG) LOGV("%s [env=%p, object=%p]", __FUNCTION__, env, object);

  const char *c_path = path ? (*env)->GetStringUTFChars(env, path, NULL) : NULL;

  if (DEBUG) LOGV("Initializing with path %s", c_path);
  int sampleRate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, BUFFER_SIZE_IN_MILLISECONDS, c_path, 0);

  if (c_path) (*env)->ReleaseStringUTFChars(env, path, c_path);

  return sampleRate;
}

JNIEXPORT jobject
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeGetVersion(
    JNIEnv *env, jclass clazz) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  return (*env)->NewStringUTF(env, espeak_Info(NULL));
}

JNIEXPORT jobjectArray
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeGetAvailableVoices(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);

  const espeak_VOICE **voices = espeak_ListVoices(NULL);

  int count;

  // First, count the number of voices returned.
  for (count = 0; voices[count] != NULL; count++);

  // Next, create a Java String array.
  jobjectArray voicesArray = (jobjectArray) (*env)->NewObjectArray(
      env, count * 4, (*env)->FindClass(env, "java/lang/String"), NULL);

  const espeak_VOICE *v;
  char gender_buf[12];
  char age_buf[12];

  // Finally, populate the array.
  for (int i = 0, voicesIndex = 0; (v = voices[i]) != NULL; i++) {
    const char *lang_name = v->languages + 1;
    const char *identifier = v->identifier;
    sprintf(gender_buf, "%d", v->gender);
    sprintf(age_buf, "%d", v->age);

    jstring lang = (*env)->NewStringUTF(env, lang_name);
    (*env)->SetObjectArrayElement(env, voicesArray, voicesIndex++, lang);
    (*env)->DeleteLocalRef(env, lang);
    jstring ident = (*env)->NewStringUTF(env, identifier);
    (*env)->SetObjectArrayElement(env, voicesArray, voicesIndex++, ident);
    (*env)->DeleteLocalRef(env, ident);
    jstring gender = (*env)->NewStringUTF(env, gender_buf);
    (*env)->SetObjectArrayElement(env, voicesArray, voicesIndex++, gender);
    (*env)->DeleteLocalRef(env, gender);
    jstring age = (*env)->NewStringUTF(env, age_buf);
    (*env)->SetObjectArrayElement(env, voicesArray, voicesIndex++, age);
    (*env)->DeleteLocalRef(env, age);
  }

  return voicesArray;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeSetVoiceByName(
    JNIEnv *env, jobject object, jstring name) {
  const char *c_name = name ? (*env)->GetStringUTFChars(env, name, NULL) : NULL;

  if (DEBUG) LOGV("%s(name=%s)", __FUNCTION__, c_name);

  const espeak_ERROR result = espeak_SetVoiceByName(c_name);

  if (c_name) (*env)->ReleaseStringUTFChars(env, name, c_name);

  switch (result) {
    case EE_OK:             return JNI_TRUE;
    case EE_INTERNAL_ERROR: LOGE("espeak_SetVoiceByName: internal error."); break;
    case EE_BUFFER_FULL:    LOGE("espeak_SetVoiceByName: buffer full."); break;
    case EE_NOT_FOUND:      LOGE("espeak_SetVoiceByName: not found."); break;
  }

  return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeSetVoiceByProperties(
    JNIEnv *env, jobject object, jstring language, jint gender, jint age) {
  const char *c_language = language ? (*env)->GetStringUTFChars(env, language, NULL) : NULL;

  if (DEBUG) LOGV("%s(language=%s, gender=%d, age=%d)", __FUNCTION__, c_language, gender, age);

  espeak_VOICE voice_select;
  memset(&voice_select, 0, sizeof(espeak_VOICE));
  voice_select.languages = c_language;
  voice_select.gender = (int) gender;
  voice_select.age = (int) age;

  const espeak_ERROR result = espeak_SetVoiceByProperties(&voice_select);

  if (c_language) (*env)->ReleaseStringUTFChars(env, language, c_language);

  switch (result) {
    case EE_OK:             return JNI_TRUE;
    case EE_INTERNAL_ERROR: LOGE("espeak_SetVoiceByProperties: internal error."); break;
    case EE_BUFFER_FULL:    LOGE("espeak_SetVoiceByProperties: buffer full."); break;
    case EE_NOT_FOUND:      LOGE("espeak_SetVoiceByProperties: not found."); break;
  }

  return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeSetParameter(
    JNIEnv *env, jobject object, jint parameter, jint value) {
  if (DEBUG) LOGV("%s(parameter=%d, value=%d)", __FUNCTION__, parameter, value);
  const espeak_ERROR result = espeak_SetParameter((espeak_PARAMETER)parameter, (int)value, 0);

  switch (result) {
    case EE_OK:             return JNI_TRUE;
    case EE_INTERNAL_ERROR: LOGE("espeak_SetParameter: internal error."); break;
    case EE_BUFFER_FULL:    LOGE("espeak_SetParameter: buffer full."); break;
    case EE_NOT_FOUND:      LOGE("espeak_SetParameter: not found."); break;
  }

  return JNI_FALSE;
}

JNIEXPORT jint
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeGetParameter(
    JNIEnv *env, jobject object, jint parameter, jint current) {
  if (DEBUG) LOGV("%s(parameter=%d, pitch=%d)", __FUNCTION__, parameter, current);
  return espeak_GetParameter((espeak_PARAMETER)parameter, (int)current);
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeSetPunctuationCharacters(
    JNIEnv *env, jobject object, jstring characters) {
  if (DEBUG) LOGV("%s)", __FUNCTION__);

  wchar_t *list = unicode_string(env, characters);
  const espeak_ERROR result = espeak_SetPunctuationList(list);
  free(list);
  switch (result) {
    case EE_OK:             return JNI_TRUE;
    case EE_INTERNAL_ERROR: LOGE("espeak_SetPunctuationList: internal error."); break;
    case EE_BUFFER_FULL:    LOGE("espeak_SetPunctuationList: buffer full."); break;
    case EE_NOT_FOUND:      LOGE("espeak_SetPunctuationList: not found."); break;
  }

  return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeSynthesize(
    JNIEnv *env, jobject object, jstring text, jboolean isSsml) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const char *c_text = text ? (*env)->GetStringUTFChars(env, text, NULL) : NULL;
  unsigned int unique_identifier;

  espeak_SetSynthCallback(SynthCallback);
  const espeak_ERROR result = espeak_Synth(c_text, strlen(c_text), 0,  // position
               POS_CHARACTER, 0, // end position (0 means no end position)
               isSsml ? espeakCHARS_UTF8 | espeakSSML // UTF-8 encoded SSML
                      : espeakCHARS_UTF8,             // UTF-8 encoded text
               &unique_identifier, object);
  espeak_Synchronize();

  if (c_text) (*env)->ReleaseStringUTFChars(env, text, c_text);

  switch (result) {
    case EE_OK:             return JNI_TRUE;
    case EE_INTERNAL_ERROR: LOGE("espeak_Synth: internal error."); break;
    case EE_BUFFER_FULL:    LOGE("espeak_Synth: buffer full."); break;
    case EE_NOT_FOUND:      LOGE("espeak_Synth: not found."); break;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean
JNICALL Java_com_reecedunn_espeak_SpeechSynthesis_nativeStop(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  espeak_Cancel();

  return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
