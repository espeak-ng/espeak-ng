/*
 * Copyright (C) 2007 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

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

	if (!fd_log)
	{
		debug_init();
	}

	if (fd_log)
	{
		fprintf(fd_log, "%03d.%03dms > ENTER %s\n",(int)(tv.tv_sec%1000), (int)(tv.tv_usec/1000), text);
	}
}


void debug_show(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (!fd_log)
	{
		debug_init();
	}
	if (fd_log)
	{
		vfprintf(fd_log, format, args);
	}
	va_end(args);
}

void debug_time(const char* text)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	if (!fd_log)
	{
		debug_init();
	}
	if (fd_log)
	{
		fprintf(fd_log, "%03d.%03dms > %s\n",(int)(tv.tv_sec%1000), (int)(tv.tv_usec/1000), text);
	}
}

#endif
