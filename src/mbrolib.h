#ifndef MBROLIB_H
#define MBROLIB_H

/*
 * mbrolib: mbrola wrapper.
 *
 * Copyright (C) 2007 Gilles Casse <gcasse@oralux.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

/* < types */

/** Parameters */

typedef struct {
  int ignore_error; /* 1=Ignore any fatal error or unknown diphone */ 
  char comment_char; /* Comment character */
  float volume_ratio; /* Volume ratio */
  float frequency_ratio; /* Applied to pitch points */
  float time_ratio; /* Applied to phone durations */
} mbrolib_parameter;


/** Returned errors */

typedef enum {
  MBROLIB_OK=0,
  MBROLIB_DATABASE_NOT_INSTALLED,
  MBROLIB_INVAL,
  MBROLIB_OUT_OF_MEMORY,
  MBROLIB_OUT_OF_RANGE,
  MBROLIB_READ_ERROR,
  MBROLIB_WRITE_ERROR
} MBROLIB_ERROR;



/** Gender */

typedef enum {
  MBROLIB_FEMALE,
  MBROLIB_MALE
} MBROLIB_GENDER;



/** Voice descriptor */

typedef struct {
  char *name; /* name (for example: "en1") */
  char *filename; /* database pathname (for example: "/usr/share/mbrola/voices/en1) */
  int rate; /* database sample rate */
  MBROLIB_GENDER gender;
  const char *language; /* Language and optional dialect qualifier in ascii (e.g. en, fr_ca). */
} mbrolib_voice;

/* > */


/** Initialization, returns a new handle.
 First function.

 @param the_sample_rate: output rate in Hz (for example 22050). If 0, keep the original database rate.

 @return handle (or NULL if error).
*/
void* mbrolib_init( int sample_rate);
typedef void* (t_mbrolib_init)(int);


/** Returns the list of the installed mbrola databases.
 The databases are searched according to the MBROLA_PATH environment variable if set, 
 or under a default path otherwise (see MBROLA_PATH in mbrolib.c).
 
 An array of voices is returned. The last item is set to NULL.
 The caller must not free the returned items or the array.

 @param the_handle previously given by mbrolib_init.

 @return An array of voices.
*/
const mbrolib_voice ** mbrolib_list_voices( void* the_handle);
typedef const mbrolib_voice ** (t_mbrolib_list_voices)(void*);



/** Set voice

 @param the_handle.

 @param the_database  (for example, "en1").

 @return error code (MBROLIB_OK, MBROLIB_DATABASE_NOT_INSTALLED, MBROLIB_INVAL).

*/
MBROLIB_ERROR mbrolib_set_voice( void* the_handle, const char* the_name);
typedef MBROLIB_ERROR (t_mbrolib_set_voice)( void*, const char*);



/** Get the current database parameters.
 The caller supplies a pointer to an already allocated structure.

 @param the_handle previously given by mbrolib_init.

 @param the_parameters: pointer to the structure.

 @return error code (MBROLIB_OK, MBROLIB_INVAL).
*/
MBROLIB_ERROR mbrolib_get_parameter(void* the_handle, mbrolib_parameter* the_parameter);
typedef MBROLIB_ERROR (t_mbrolib_get_parameter)(void*, mbrolib_parameter*);



/** Set the database parameters using the supplied data.

 @param the_handle previously given by mbrolib_init.

 @param the_parameters: pointer to the wished parameters.

 @return error code (MBROLIB_OK, MBROLIB_INVAL).
*/
MBROLIB_ERROR mbrolib_set_parameter(void* the_handle, const mbrolib_parameter* the_parameter);
typedef MBROLIB_ERROR (t_mbrolib_set_parameter)(void*, const mbrolib_parameter*);



/** Write the mbrola phonemes in the internal buffer.

 @param the_handle.

 @param the_mbrola_phonemes.

 @param the_size in bytes.

 @return error code (MBROLIB_OK, MBROLIB_INVAL, MBROLIB_WRITE_ERROR, MBROLIB_READ_ERROR).
*/
MBROLIB_ERROR mbrolib_write(void* the_handle, const char* the_mbrola_phonemes, size_t the_size);
typedef MBROLIB_ERROR (t_mbrolib_write)(void*, const char*, size_t);



/** Read n bytes of the output samples.

 @param the_handle.

 @param the_samples (raw audio data, 16bits, mono).

 @param the_size max number of int16 to read.

 @param the_size number of int16 read.

 @return error code (MBROLIB_OK, MBROLIB_INVAL, MBROLIB_READ_ERROR).

*/
MBROLIB_ERROR mbrolib_read(void* the_handle, short* the_samples, int the_max_size, int* the_read_size);
typedef MBROLIB_ERROR (t_mbrolib_read)(void*, short*, int, int*);



/** Flush

 @param the_handle.

*/
void mbrolib_flush(void* the_handle);
typedef void (t_mbrolib_flush)(void*);



/** Release the handle

 @param the_handle.

 @return error code (MBROLIB_OK, MBROLIB_INVAL).

*/
MBROLIB_ERROR mbrolib_terminate(void* the_handle);
typedef MBROLIB_ERROR (t_mbrolib_terminate)(void*);



#ifdef __cplusplus
}
#endif

#endif
