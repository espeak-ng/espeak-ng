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

#define PLATFORM_DOS
#define NEED_WCHAR_FUNCTIONS
#define NEED_GETOPT
typedef unsigned int  uint32_t;

#define PATHSEP  '\\'
//#define USE_PORTAUDIO
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


#ifdef PLATFORM_DOS
int iswalpha(int c);
int iswdigit(int c);
int iswalnum(int c);
int towlower(int c);
int iswupper(int c);
int iswlower(int c);
int iswspace(int c);
int iswpunct(int c);
const wchar_t *wcschr(const wchar_t *str, int c);
const int wcslen(const wchar_t *str);
float wcstod(const wchar_t *str, wchar_t **tailptr);
int towupper(int c);
#endif



typedef unsigned short USHORT;
typedef unsigned char  UCHAR;
typedef double DOUBLEX;




typedef struct {
   const char *mnem;
   int  value;
} MNEM_TAB;
int LookupMnem(MNEM_TAB *table, char *string);



#define N_PATH_HOME  120
extern char path_home[N_PATH_HOME];    // this is the espeak-data directory

extern void strncpy0(char *to,const char *from, int size);
int  GetFileLength(const char *filename);
char *Alloc(int size);
void Free(void *ptr);

