/*
 * Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>
 * Copyright (C) 2015-2016 Reece H. Dunn
 * based on AudioIO.cc (Audacity-1.2.4b) and wavegen.cpp
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

#ifndef WAVE_H
#define WAVE_H

#ifdef __cplusplus
extern "C"
{
#endif

extern void *wave_open(int samplerate, const char *device);
extern size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize);
extern int wave_close(void *theHandler);
extern void wave_flush(void *theHandler);
extern void wave_terminate();

// general functions
extern void clock_gettime2(struct timespec *ts);
extern void add_time_in_ms(struct timespec *ts, int time_in_ms);

#ifdef __cplusplus
}
#endif

#endif
