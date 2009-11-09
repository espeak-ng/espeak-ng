#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define ENTER(text) debug_enter(text)
#define SHOW(format,...) debug_show(format,__VA_ARGS__);
#define SHOW_TIME(text) debug_time(text);
extern void debug_enter(const char* text);
extern void debug_show(const char* format,...);
extern void debug_time(const char* text);

#else

#ifdef NO_VARIADIC_MACROS
#define SHOW(format)   // VC6 doesn't allow "..."
#else
#define SHOW(format,...)
#endif
#define SHOW_TIME(text)
#define ENTER(text)
#endif


#endif
