/*
 * mbrowrap -- A wrapper library around the mbrola binary
 * providing a subset of the API from the Windows mbrola DLL.
 *
 * Copyright (C) 2010 by Nicolas Pitre <nico@fluxnic.net>
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
 */

#ifndef MBROWRAP_H
#define MBROWRAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Initialize mbrola.  The 'voice_path' argument must contain the
 * path and file name to the mbrola voice database to be used. Returned
 * value is 0 on success, or an error code otherwise (currently only -1
 * is returned. If not successful, lastErrorStr_MBR() will provide the
 * error reason.  If this is successful, then close_MBR() must be called
 * before init_MBR() can be called again.
 */
int init_MBR(const char *voice_path);

/*
 * Stop mbrola and release any resources.  It is necessary to call
 * this after a successful call to init_MBR() before init_MBR() can be
 * called again.
 */
void close_MBR(void);

/*
 * Stop any ongoing processing and flush all buffers.  After this call
 * any synthesis request will start afresh.  A non-zero value is returned
 * on success, or 0 on failure. If not successful, lastErrorStr_MBR() will
 * provide the error reason.
 */
int reset_MBR();

/*
 * Return at most 'nb_samples' audio samples into 'buffer'. The returned
 * value is the actual number of samples returned, or -1 on error.
 * If not successful, lastErrorStr_MBR() will provide the error reason.
 * Samples are always 16-bit little endian.
 */
int read_MBR(void *buffer, int nb_samples);

/*
 * Write a NULL terminated string of phoneme in the input buffer.
 * Return the number of chars actually written, or -1 on error.
 * If not successful, lastErrorStr_MBR() will provide the error reason.
 */
int write_MBR(const char *data);

/*
 * Send a flush command to the mbrola input stream.
 * This is currently similar to write_MBR("#\n").  Return 1 on success
 * or 0 on failure. If not successful, lastErrorStr_MBR() will provide
 * the error reason.
 */
int flush_MBR(void);

/*
 * Return the audio sample frequency of the used voice database.
 */
int getFreq_MBR(void);

/*
 * Overall volume.
 */
void setVolumeRatio_MBR(float value);

/*
 * Copy into 'buffer' at most 'bufsize' bytes from the latest error
 * message.  This may also contain non-fatal errors from mbrola.  The
 * returned value is the actual number of bytes copied.  When no error
 * message is pending then an empty string is returned.  Consecutive
 * calls to lastErrorStr_MBR() will return the same message unless it
 * is explicitly cleared with resetError_MBR().
 */
int lastErrorStr_MBR(char *buffer, int bufsize);

/*
 * Clear any pending error message.
 */
void resetError_MBR(void);

/*
 * Tolerance to missing diphones (always active so this is ignored)
 */
static inline void setNoError_MBR(int no_error) { }

#ifdef __cplusplus
}
#endif

#endif
