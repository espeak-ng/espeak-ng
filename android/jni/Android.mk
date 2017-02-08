LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS = -std=c11

# ucd-tools wide-character compatibility support:

UCDTOOLS_SRC_PATH  := ../../ucd-tools/src
UCDTOOLS_SRC_FILES := \
  $(subst $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH),$(UCDTOOLS_SRC_PATH),$(wildcard $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH)/*.c*))

LOCAL_SRC_FILES += $(UCDTOOLS_SRC_FILES)

ESPEAK_SOURCES := \
  src/libespeak-ng/compiledata.c \
  src/libespeak-ng/compiledict.c \
  src/libespeak-ng/compilembrola.c \
  src/libespeak-ng/dictionary.c \
  src/libespeak-ng/error.c \
  src/libespeak-ng/espeak_api.c \
  src/libespeak-ng/ieee80.c \
  src/libespeak-ng/intonation.c \
  src/libespeak-ng/klatt.c \
  src/libespeak-ng/numbers.c \
  src/libespeak-ng/phoneme.c \
  src/libespeak-ng/phonemelist.c \
  src/libespeak-ng/readclause.c \
  src/libespeak-ng/setlengths.c \
  src/libespeak-ng/spect.c \
  src/libespeak-ng/speech.c \
  src/libespeak-ng/synthdata.c \
  src/libespeak-ng/synthesize.c \
  src/libespeak-ng/synth_mbrola.c \
  src/libespeak-ng/translate.c \
  src/libespeak-ng/tr_languages.c \
  src/libespeak-ng/voices.c \
  src/libespeak-ng/wavegen.c

ESPEAK_SRC_PATH  := ../../src
ESPEAK_SRC_FILES := \
  $(subst src/,$(ESPEAK_SRC_PATH)/,$(ESPEAK_SOURCES))

LOCAL_CFLAGS    += -DINCLUDE_KLATT -DINCLUDE_SONIC
LOCAL_SRC_FILES += \
  $(filter-out $(BLACKLIST_SRC_FILES),$(ESPEAK_SRC_FILES))

# JNI

LOCAL_SRC_FILES += \
  $(subst $(LOCAL_PATH)/jni,jni,$(wildcard $(LOCAL_PATH)/jni/*.c))

# Common

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH)/include \
  $(LOCAL_PATH)/$(ESPEAK_SRC_PATH)/include

LOCAL_LDLIBS := \
  -llog

LOCAL_MODULE := libttsespeak
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
