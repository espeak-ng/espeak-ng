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

// Windows command-line version of eSpeak

#ifndef SPEECH_H
#define SPEECH_H



// conditional compilation options

#define PLATFORM_WINDOWS
#define __WIN32__
#define NEED_WCSTOF
#define NEED_GETOPT

#define PATHSEP '\\'
// #define USE_PORTAUDIO
#define NO_VARIADIC_MACROS
#define ESPEAK_API __declspec(dllexport)
#define LIBRARY
#define INCLUDE_KLATT
#define INCLUDE_MBROLA
#define INCLUDE_SONIC

// will look for espeak_data directory here, and also in user's home directory
#define PATH_ESPEAK_DATA  "/usr/share/espeak-data"

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

#endif // SPEECH_H
