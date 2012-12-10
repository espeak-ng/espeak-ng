LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# eSpeak (minus command line apps)

BLACKLIST_SRC_FILES := \
  %/espeak.cpp \
  %/speak.cpp

ESPEAK_SRC_PATH := espeak-source/src

ESPEAK_SRC_FILES := \
  $(subst $(LOCAL_PATH)/$(ESPEAK_SRC_PATH),$(ESPEAK_SRC_PATH),$(wildcard $(LOCAL_PATH)/$(ESPEAK_SRC_PATH)/*.c*))
  
LOCAL_SRC_FILES += \
  $(filter-out $(BLACKLIST_SRC_FILES),$(ESPEAK_SRC_FILES))

# JNI

LOCAL_SRC_FILES += \
  $(subst $(LOCAL_PATH)/jni,jni,$(wildcard $(LOCAL_PATH)/jni/*.c*))

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/$(ESPEAK_SRC_PATH)

LOCAL_LDLIBS := \
  -llog

# Common

LOCAL_MODULE := libttsespeak
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
