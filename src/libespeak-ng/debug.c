#include <stdio.h>
#include <stdarg.h>
#include "speech.h"
#include "debug.h"

#ifdef DEBUG_ENABLED
#include <sys/time.h>
#include <unistd.h>

static FILE* fd_log=NULL;
static const char* FILENAME="/tmp/espeak.log";

void debug_init()
{
	if((fd_log = fopen(FILENAME,"a")) != NULL)
		setvbuf(fd_log, NULL, _IONBF, 0);
}

void debug_enter(const char* text)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);                  

  //  fd_log = fopen(FILENAME,"a");
  if (!fd_log)
    {
      debug_init();
    }

  if (fd_log)
    {
      fprintf(fd_log, "%03d.%03dms > ENTER %s\n",(int)(tv.tv_sec%1000), (int)(tv.tv_usec/1000), text);
      //      fclose(fd_log);
    }
}


void debug_show(const char *format, ...)
{
  va_list args;		
  va_start(args, format);
  //  fd_log = fopen(FILENAME,"a");
  if (!fd_log)
    {
      debug_init();
    }
  if (fd_log)
    {
      vfprintf(fd_log, format, args);
      //      fclose(fd_log);
    }  
  va_end(args);
}

void debug_time(const char* text)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);                  

  //  fd_log = fopen(FILENAME,"a");
  if (!fd_log)
    {
      debug_init();
    }
  if (fd_log)
    {
      fprintf(fd_log, "%03d.%03dms > %s\n",(int)(tv.tv_sec%1000), (int)(tv.tv_usec/1000), text);
      //      fclose(fd_log);
    }
}

#endif
