/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/


#include <sys/types.h>

// conditional compilation options

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#define ARCH_BIG
#endif

#define PLATFORM_POSIX
#define PATHSEP  '/'
#define USE_PORTAUDIO
#define USE_NANOSLEEP
#define __cdecl 
#define ESPEAK_API  extern "C"

#ifdef LIBRARY
#define USE_ASYNC
//#define USE_MBROLA_LIB
#endif

#ifdef _ESPEAKEDIT
#define USE_ASYNC
#define LOG_FRAMES      // write keyframe info to log-espeakedit
#endif

// will look for espeak_data directory here, and also in user's home directory
#ifndef PATH_ESPEAK_DATA
   #define PATH_ESPEAK_DATA  "/usr/share/espeak-data"
#endif

typedef unsigned short USHORT;
typedef unsigned char  UCHAR;
typedef double DOUBLEX;



#define N_PEAKS   9
#define N_MARKERS 8


typedef struct {
   const char *mnem;
   int  value;
} MNEM_TAB;
int LookupMnem(MNEM_TAB *table, char *string);


typedef struct {
   short pkfreq;
   short pkheight;
   short pkwidth;
   short pkright;
}  peak_t;

typedef struct {
	short frflags;
	unsigned char length;
	unsigned char rms;
	short ffreq[9];
	unsigned char fheight[9];
	unsigned char fwidth[6];          // width/4
	unsigned char fright[6];          // width/4
} frame_t;


int  GetFileLength(const char *filename);
char *Alloc(int size);
void Free(void *ptr);

#define N_PATH_HOME  120
extern char path_home[N_PATH_HOME];    // this is the espeak-data directory

extern const char *version_string;
extern const int version_phdata;
extern void strncpy0(char *to,const char *from, int size);

