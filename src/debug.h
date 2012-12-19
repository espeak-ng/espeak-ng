#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG_ENABLED

#ifdef ANDROID

#include <android/log.h>
#define LOG_TAG "eSpeak"

#ifdef DEBUG_ENABLED
#define ENTER(text) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, text)
#define SHOW(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define ENTER(text)
#define SHOW(format,...)
#endif

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define SHOW_TIME(text)

#else

#ifdef DEBUG_ENABLED

#define ENTER(text) debug_enter(text)
#define SHOW(format,...) debug_show(format,__VA_ARGS__);
#define LOGI(format,...) debug_show(format,__VA_ARGS__);
#define LOGE(format,...) debug_show(format,__VA_ARGS__);
#define SHOW_TIME(text) debug_time(text);

extern void debug_enter(const char* text);
extern void debug_show(const char* format,...);
extern void debug_time(const char* text);

#else

#ifdef NO_VARIADIC_MACROS
#define SHOW(format)   // VC6 doesn't allow "..."
#define LOGI(format)   // VC6 doesn't allow "..."
#define LOGE(format)   // VC6 doesn't allow "..."
#else
#define SHOW(format,...)
#define LOGI(format,...)
#define LOGE(format,...)
#endif

#define SHOW_TIME(text)
#define ENTER(text)

#endif
#endif
#endif
