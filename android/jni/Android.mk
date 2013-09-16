LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS = -std=c++11

# ucd-tools wide-character compatibility support:

UCDTOOLS_SRC_PATH  := ../../ucd-tools/src
UCDTOOLS_SRC_FILES := \
  $(subst $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH),$(UCDTOOLS_SRC_PATH),$(wildcard $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH)/*.c*))

LOCAL_SRC_FILES += $(UCDTOOLS_SRC_FILES)

# eSpeak (minus command line apps and espeakedit)

ESPEAK_SOURCES := \
  src/compiledict.cpp \
  src/debug.cpp \
  src/dictionary.cpp \
  src/intonation.cpp \
  src/klatt.cpp \
  src/numbers.cpp \
  src/readclause.cpp \
  src/phonemelist.cpp \
  src/setlengths.cpp \
  src/sonic.cpp \
  src/speak_lib.cpp \
  src/synthdata.cpp \
  src/synthesize.cpp \
  src/synth_mbrola.cpp \
  src/translate.cpp \
  src/tr_languages.cpp \
  src/voices.cpp \
  src/wavegen.cpp \
  src/wave.cpp

ESPEAK_SRC_PATH  := ../../src
ESPEAK_SRC_FILES := \
  $(subst src/,$(ESPEAK_SRC_PATH)/,$(ESPEAK_SOURCES))

LOCAL_CFLAGS    += -DINCLUDE_KLATT -DINCLUDE_SONIC
LOCAL_SRC_FILES += \
  $(filter-out $(BLACKLIST_SRC_FILES),$(ESPEAK_SRC_FILES))

# JNI

LOCAL_SRC_FILES += \
  $(subst $(LOCAL_PATH)/jni,jni,$(wildcard $(LOCAL_PATH)/jni/*.c*))

# Common

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/$(UCDTOOLS_SRC_PATH)/include \
  $(LOCAL_PATH)/$(ESPEAK_SRC_PATH)

LOCAL_LDLIBS := \
  -llog

LOCAL_MODULE := libttsespeak
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
