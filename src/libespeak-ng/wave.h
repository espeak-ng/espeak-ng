/*
 * Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>
 * Copyright (C) 2015 Reece H. Dunn
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

#ifndef PLATFORM_DOS
#include "stdint.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern int option_device_number;

extern int wave_init(int samplerate);
extern void *wave_open(const char *the_api);

extern size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize);
extern int wave_close(void *theHandler);
extern void wave_flush(void *theHandler);
extern int wave_is_busy(void *theHandler);
extern void wave_terminate();
extern uint32_t wave_get_read_position(void *theHandler);
extern uint32_t wave_get_write_position(void *theHandler);

// Supply the remaining time in ms before the sample is played
// (or 0 if the event has been already played).
// sample: sample identifier
// time: supplied value in ms
//
// return 0 if ok or -1 otherwise (stream not opened).
extern int wave_get_remaining_time(uint32_t sample, uint32_t *time);

// set the callback which informs if the output is still enabled.
// Helpful if a new sample is waiting for free space whereas sound must be stopped.
typedef int (t_wave_callback)(void);
extern void wave_set_callback_is_output_enabled(t_wave_callback *cb);


// general functions
extern void clock_gettime2(struct timespec *ts);
extern void add_time_in_ms(struct timespec *ts, int time_in_ms);

// for tests
extern void *wave_test_get_write_buffer();

#ifdef __cplusplus
}
#endif

#endif
