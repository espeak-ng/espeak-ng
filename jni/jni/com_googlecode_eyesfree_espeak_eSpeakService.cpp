/*
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jni.h>

#include <speak_lib.h>
#include <TtsEngine.h>
#include <Log.h>

#define LOG_TAG "eSpeakService"
#define DEBUG false

enum audio_channel_count {
  CHANNEL_COUNT_MONO = 1,
  CHANNEL_COUNT_STEREO = 2
};

enum audio_encoding {
  ENCODING_INVALID = 0x00,
  ENCODING_DEFAULT = 0x01,
  ENCODING_PCM_16BIT = 0x02,
  ENCODING_PCM_8BIT = 0x03
};

enum synthesis_result {
  SYNTH_CONTINUE = 0,
  SYNTH_ABORT = 1
};

const int DEFAULT_SAMPLE_RATE = 22050;
const int DEFAULT_CHANNEL_COUNT = CHANNEL_COUNT_MONO;
const int DEFAULT_AUDIO_FORMAT = ENCODING_PCM_16BIT;
const int DEFAULT_BUFFER_SIZE = 1000;

struct native_data_t {
  JNIEnv *env;
  jobject object;
  int sampleRate;
  int channelCount;
  int audioFormat;
  int bufferSizeInMillis;

  native_data_t() {
    env = NULL;
    object = NULL;
    sampleRate = DEFAULT_SAMPLE_RATE;
    channelCount = DEFAULT_CHANNEL_COUNT;
    audioFormat = DEFAULT_AUDIO_FORMAT;
    bufferSizeInMillis = DEFAULT_BUFFER_SIZE;
  }
};

jmethodID METHOD_nativeSynthCallback;
jfieldID FIELD_mNativeData;

static inline native_data_t *getNativeData(JNIEnv *env, jobject object) {
  return (native_data_t *) (env->GetIntField(object, FIELD_mNativeData));
}

/* Callback from espeak.  Should call back to the TTS API */
static int SynthCallback(short *audioData, int numSamples,
                         espeak_EVENT *events) {
  native_data_t *nat = (native_data_t *) events->user_data;
  JNIEnv *env = nat->env;
  jobject object = nat->object;

  if (numSamples < 1) {
    env->CallVoidMethod(object, METHOD_nativeSynthCallback, NULL);
    return SYNTH_ABORT;
  } else {
    jbyteArray arrayAudioData = env->NewByteArray(numSamples * 2);
    env->SetByteArrayRegion(arrayAudioData, 0, (numSamples * 2), (jbyte *) audioData);
    env->CallVoidMethod(object, METHOD_nativeSynthCallback, arrayAudioData);
    return SYNTH_CONTINUE;
  }
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

JNIEXPORT jint
JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env;

  if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    LOGE("Failed to get the environment using GetEnv()");
    return -1;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeClassInit(
    JNIEnv* env, jclass clazz) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  METHOD_nativeSynthCallback = env->GetMethodID(clazz, "nativeSynthCallback", "([B)V");
  FIELD_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");

  return JNI_TRUE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeCreate(
    JNIEnv *env, jobject object, jstring path) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  native_data_t *nat = new native_data_t;

  if (nat == NULL) {
    LOGE("%s: out of memory!", __FUNCTION__);
    return JNI_FALSE;
  }

  env->SetIntField(object, FIELD_mNativeData, (jint) nat);

  const char *c_path = env->GetStringUTFChars(path, NULL);

  nat->object = env->NewWeakGlobalRef(object);
  if (DEBUG) LOGV("Initializing with path %s", c_path);
  nat->sampleRate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, nat->bufferSizeInMillis, c_path, 0);

  env->ReleaseStringUTFChars(path, c_path);

  if (nat->sampleRate > 0) {
    return JNI_TRUE;
  } else {
    return JNI_FALSE;
  }
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeDestroy(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  native_data_t *nat = getNativeData(env, object);

  env->DeleteWeakGlobalRef(nat->object);

  free(nat);

  return JNI_TRUE;
}

JNIEXPORT jint
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeGetSampleRate(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const native_data_t *nat = getNativeData(env, object);
  return (jint) nat->sampleRate;
}

JNIEXPORT jint
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeGetChannelCount(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const native_data_t *nat = getNativeData(env, object);
  return (jint) nat->channelCount;
}

JNIEXPORT jint
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeGetAudioFormat(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const native_data_t *nat = getNativeData(env, object);
  return (jint) nat->audioFormat;
}

JNIEXPORT jint
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeGetBufferSizeInMillis(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const native_data_t *nat = getNativeData(env, object);
  return (jint) nat->bufferSizeInMillis;
}

JNIEXPORT jobjectArray
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeGetAvailableVoices(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);

  const espeak_VOICE **voices = espeak_ListVoices(NULL);

  int count;

  // First, count the number of voices returned.
  for (count = 0; voices[count] != NULL; count++);

  // Next, create a Java String array.
  jobjectArray voicesArray = (jobjectArray) env->NewObjectArray(
      count * 4, env->FindClass("java/lang/String"), NULL);

  const espeak_VOICE *v;
  char gender_buf[12];
  char age_buf[12];

  // Finally, populate the array.
  for (int i = 0, voicesIndex = 0; (v = voices[i]) != NULL; i++) {
    const char *lang_name = v->languages + 1;
    const char *identifier = v->identifier;
    sprintf(gender_buf, "%d", v->gender);
    sprintf(age_buf, "%d", v->age);

    env->SetObjectArrayElement(
        voicesArray, voicesIndex++, env->NewStringUTF(lang_name));
    env->SetObjectArrayElement(
        voicesArray, voicesIndex++, env->NewStringUTF(identifier));
    env->SetObjectArrayElement(
        voicesArray, voicesIndex++, env->NewStringUTF(gender_buf));
    env->SetObjectArrayElement(
        voicesArray, voicesIndex++, env->NewStringUTF(age_buf));
  }

  return voicesArray;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeSetVoiceByProperties(
    JNIEnv *env, jobject object, jstring name, jstring languages, jint gender, jint age,
    jint variant) {
  if (DEBUG) LOGV("%s", __FUNCTION__);

  const char *c_name = env->GetStringUTFChars(name, NULL);
  const char *c_languages = env->GetStringUTFChars(languages, NULL);

  espeak_VOICE voice_select;
  memset(&voice_select, 0, sizeof(espeak_VOICE));

  voice_select.name = c_name;
  voice_select.languages = c_languages;
  voice_select.age = (int) age;
  voice_select.gender = (int) gender;
  voice_select.variant = (int) variant;

  const espeak_ERROR result = espeak_SetVoiceByProperties(&voice_select);

  env->ReleaseStringUTFChars(name, c_name);
  env->ReleaseStringUTFChars(languages, c_languages);

  if (result == EE_OK)
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeSetLanguage(
    JNIEnv *env, jobject object, jstring language, jint variant) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const char *c_language = env->GetStringUTFChars(language, NULL);
  const int len = strlen(c_language);
  char *lang_copy = (char *) calloc(len, sizeof(char));
  strcpy(lang_copy, c_language);
  env->ReleaseStringUTFChars(language, c_language);

  espeak_VOICE voice;
  memset(&voice, 0, sizeof(espeak_VOICE));  // Zero out the voice first
  voice.languages = lang_copy;
  voice.variant = (int) variant;
  const espeak_ERROR result = espeak_SetVoiceByProperties(&voice);

  if (result == EE_OK)
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeSetRate(
    JNIEnv *env, jobject object, jint rate) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const espeak_ERROR result = espeak_SetParameter(espeakRATE, (int) rate, 0);

  if (result == EE_OK)
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeSetPitch(
    JNIEnv *env, jobject object, jint pitch) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  const espeak_ERROR result = espeak_SetParameter(espeakPITCH, (int) pitch, 0);

  if (result == EE_OK)
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeSynthesize(
    JNIEnv *env, jobject object, jstring text) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  native_data_t *nat = getNativeData(env, object);
  const char *c_text = env->GetStringUTFChars(text, NULL);
  unsigned int unique_identifier;

  nat->env = env;

  espeak_SetSynthCallback(SynthCallback);
  espeak_Synth(c_text, strlen(c_text), 0,  // position
               POS_CHARACTER, 0,  // end position (0 means no end position)
               espeakCHARS_UTF8 | espeakSSML, // use or ignore xml tags
               &unique_identifier, nat);
  espeak_Synchronize();

  env->ReleaseStringUTFChars(text, c_text);

  return JNI_TRUE;
}

JNIEXPORT jboolean
JNICALL Java_com_googlecode_eyesfree_espeak_SpeechSynthesis_nativeStop(
    JNIEnv *env, jobject object) {
  if (DEBUG) LOGV("%s", __FUNCTION__);
  espeak_Cancel();

  return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
