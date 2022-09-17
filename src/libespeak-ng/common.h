/*
 * Copyright (C) 2005 to 2007 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2015 Reece H. Dunn
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

#ifndef ESPEAK_NG_COMMON_H
#define ESPEAK_NG_COMMON_H

#include "espeak-ng/espeak_ng.h"

extern ESPEAK_NG_API int GetFileLength(const char *filename);
extern ESPEAK_NG_API void strncpy0(char *to, const char *from, int size);

ESPEAK_NG_API int utf8_in(int *c, const char *buf);
int utf8_in2(int *c, const char *buf, int backwards);
int utf8_out(unsigned int c, char *buf);

#ifdef __cplusplus
}
#endif

#endif // SPEECH_H
