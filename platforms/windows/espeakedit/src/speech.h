/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
 *   jsd@clara.co.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SPEECH_H
#define SPEECH_H

#define LOG_FRAMES      // write keyframe info to src/log


// comment this out when compiling the "speak" process
//#define SPECT_EDITOR
#define INCLUDE_KLATT
#define INCLUDE_MBROLA

#include <windows.h>
#define PLATFORM_WINDOWS
#define __WIN32__
#define mkdir(p1,p2) mkdir(p1)

#define PATHSEP  '\\'
#define USE_PORTAUDIO
//#define USE_NANOSLEEP
#define NO_VARIADIC_MACROS
#define __cdecl 
#define PATH_ESPEAK_DATA  "C:\\Program files\\espeak\\espeak-data"

typedef unsigned short USHORT;
typedef unsigned char  UCHAR;
typedef double DOUBLEX;
#ifdef __WIN64__
typedef uint64_t long64;  // use this for conversion between pointers and integers
#else
typedef unsigned long long64;
#endif




typedef struct {
   const char *mnem;
   int  value;
} MNEM_TAB;
int LookupMnem(MNEM_TAB *table, const char *string);



#define N_PATH_HOME  230
extern char path_home[N_PATH_HOME];    // this is the espeak-data directory

extern void strncpy0(char *to,const char *from, int size);
int  GetFileLength(const char *filename);
char *Alloc(int size);
void Free(void *ptr);

#endif SPEECH_H
