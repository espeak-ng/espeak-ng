/***************************************************************************
 *   Copyright (C) 2008, Sun Microsystems, Inc.                            *
 *   eSpeak driver for Solaris Audio Device Architecture (SADA)            *
 *   Written by Willie Walker, based on the eSpeak PulseAudio driver       *
 *   from Gilles Casse                                                     *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "speech.h"

#ifdef USE_ASYNC
// This source file is only used for asynchronious modes

#include <errno.h>
#include <string.h>
#include <stropts.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/audioio.h>

#include "wave.h"
#include "debug.h"

enum {ONE_BILLION=1000000000};
#define SAMPLE_RATE 22050
#define SAMPLE_SIZE 16

#ifdef USE_SADA

static t_wave_callback* my_callback_is_output_enabled=NULL;

static const char *sun_audio_device = "/dev/audio";
static int sun_audio_fd = -1;

// The total number of 16-bit samples sent to be played via the
// wave_write method.
//
static uint32_t total_samples_sent;

// The total number of samples sent to be played via the wave_write
// method, but which were never played because of a call to
// wave_close.
//
static uint32_t total_samples_skipped;

// The last known playing index after a call to wave_close.
//
static uint32_t last_play_position=0;

static uint32_t wave_samplerate;
//>
// wave_init 
//
// DESCRIPTION:
//
// initializes the audio subsytem.
//
// GLOBALS USED/MODIFIED:
//
// sun_audio_fd: modified to hold the file descriptor of the opened
// audio device.
//
//<wave_init

int wave_init(int srate) {
  ENTER("wave_init");

  audio_info_t ainfo;
  char *audio_device = NULL;

	wave_samplerate = srate;

  audio_device = getenv("AUDIODEV");
  if (audio_device != NULL) {
    if ((sun_audio_fd = open(audio_device, O_WRONLY)) < 0) {
      SHOW("wave_init() could not open: %s (%d)\n", 
	   audio_device, sun_audio_fd);
    }
  }

  if (sun_audio_fd < 0) {
    if ((sun_audio_fd = open(sun_audio_device, O_WRONLY)) < 0) {
      SHOW("wave_init() could not open: %s (%d)\n", 
           sun_audio_device, sun_audio_fd);
    }
  }

  SHOW("wave_init() sun_audio_fd: %d\n", sun_audio_fd);

  if (sun_audio_fd < 0) {
    return(0);
  }

  ioctl(sun_audio_fd, AUDIO_GETINFO, &ainfo);
  SHOW("wave_init() play buffer size: %d\n", ainfo.play.buffer_size);
  ainfo.play.encoding = AUDIO_ENCODING_LINEAR;
  ainfo.play.channels = 1;
  ainfo.play.sample_rate = wave_samplerate;
  ainfo.play.precision = SAMPLE_SIZE;

  if (ioctl(sun_audio_fd, AUDIO_SETINFO, &ainfo) == -1) {
    SHOW("wave_init() failed to set audio params: %s\n", strerror(errno));
    close(sun_audio_fd);
    return(0);
  }
  return(1);
}

//>
// wave_open
//
// DESCRIPTION:
//
// opens the audio subsystem given a specific API (e.g., "alsa",
// "oss", ...).  We ignore the_api and just return the sun_audio_fd we
// opened in wave_init.  This return value will be passed in as the
// theHandler parameter in all other methods.
//
// PARAMETERS:
//
// the_api: "alsa", "oss" (ignored)
//
// GLOBALS USED/MODIFIED:
//
// sun_audio_fd: used as return value
//
// RETURNS:
//
// sun_audio_fd opened in wave_init, which is passed in as theHandler
// parameter in all other methods
//
//<wave_open

void* wave_open(const char* the_api)
{
  ENTER("wave_open");
  return((void*) sun_audio_fd);
}

//>
// wave_write
//
// DESCRIPTION:
//
// Meant to be asynchronous, it supplies the wave sample to the lower
// audio layer and returns. The sample is played later on.  [[[WDW -
// we purposely do not open the audio device as non-blocking because
// managing that would be a pain.  So, we rely a lot upon fifo.cpp and
// event.cpp to not overload us, allowing us to get away with a
// blocking write.  event.cpp:polling_thread in particular appears to
// use get_remaining_time to prevent flooding.]]]
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
// theMono16BitsWaveBuffer: the audio data
// theSize: the number of bytes (not 16-bit samples)
//
// GLOBALS USED/MODIFIED:
//
// total_samples_sent: modified based upon 16-bit samples sent
//
// RETURNS: 
//
// the number of bytes (not 16-bit samples) sent
//
//<wave_write

size_t wave_write(void* theHandler, 
		  char* theMono16BitsWaveBuffer, 
		  size_t theSize)
{
  size_t num;
  ENTER("wave_write");
  if (my_callback_is_output_enabled && (0==my_callback_is_output_enabled())) {
    SHOW_TIME("wave_write > my_callback_is_output_enabled: no!");
    return 0;
  }

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
  {
    // BIG-ENDIAN, swap the order of bytes in each sound sample
    int c;
    char *out_ptr;
    char *out_end;
    out_ptr = (char *)theMono16BitsWaveBuffer;
    out_end = out_ptr + theSize;
    while(out_ptr < out_end)
    {
      c = out_ptr[0];
      out_ptr[0] = out_ptr[1];
      out_ptr[1] = c;
      out_ptr += 2;
    }
  }
#endif

  num = write((int) theHandler, theMono16BitsWaveBuffer, theSize);

  // Keep track of the total number of samples sent -- we use this in 
  // wave_get_read_position and also use it to help calculate the
  // total_samples_skipped in wave_close.
  //
  total_samples_sent += num / 2;

  if (num < theSize) {
    SHOW("ERROR: wave_write only wrote %d of %d bytes\n", num, theSize);
  } else {
    SHOW("wave_write wrote %d bytes\n", theSize);
  }

  SHOW_TIME("wave_write > LEAVE");
  return num;
}

//>
// wave_close
//
// DESCRIPTION:
//
// Does what SADA normally would call a flush, which means to cease
// all audio production in progress and throw any remaining audio
// away.  [[[WDW - see comment in wave_flush.]]]
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
//
// GLOBALS USED/MODIFIED:
//
// last_play_position: modified to reflect play position the last time
//                     this method was called
// total_samples_sent: used to help calculate total_samples_skipped
// total_samples_skipped: modified to hold the total number of 16-bit
//                        samples sent to wave_write, but which were
//                        never played
// sun_audio_fd: used because some calls to wave_close seem to 
//               pass a NULL for theHandler for some odd reason
//
// RETURNS: 
//
// The result of the ioctl call (non-0 means failure)
//
//<wave_close

int wave_close(void* theHandler)
{
  int ret;
  audio_info_t ainfo;
  int audio_fd = (int) theHandler;
  if (!audio_fd) {
    audio_fd = sun_audio_fd;
  }
  ENTER("wave_close");
  // [[[WDW: maybe do a pause/resume ioctl???]]]
  ret = ioctl(audio_fd, I_FLUSH, FLUSHRW);
  ioctl(audio_fd, AUDIO_GETINFO, &ainfo);

  // Calculate the number of samples that won't get
  // played.  We also keep track of the last_play_position
  // because wave_close can be called multiple times
  // before another call to wave_write.
  //
  if (last_play_position != ainfo.play.samples) {
    last_play_position = ainfo.play.samples;
    total_samples_skipped = total_samples_sent - last_play_position;
  }
  SHOW_TIME("wave_close > LEAVE");
  return ret;
}

//>
// wave_is_busy
//
// DESCRIPTION:
//
// Returns a non-0 value if audio is being played.
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
//
// GLOBALS USED/MODIFIED:
//
// sun_audio_fd: used because some calls to wave_is_busy seem to 
//               pass a NULL for theHandler for some odd reason
//
// RETURNS:
//
// A non-0 value if audio is being played
//
//<wave_is_busy

int wave_is_busy(void* theHandler)
{
   uint32_t time;
   if (total_samples_sent >= 1) {
       wave_get_remaining_time(total_samples_sent - 1, &time);
   } else {
       time = 0;
   }
   return time != 0;
}

//>
// wave_terminate
//
// DESCRIPTION:
//
// Used to end our session with eSpeak.
//
// GLOBALS USED/MODIFIED:
//
// sun_audio_fd: modified - closed and set to -1
//
//<wave_terminate

void wave_terminate()
{
  ENTER("wave_terminate");
  close(sun_audio_fd);
  sun_audio_fd = -1;
  SHOW_TIME("wave_terminate > LEAVE");
}

//>
// wave_flush
//
// DESCRIPTION:
//
// Appears to want to tell the audio subsystem to make sure it plays
// the audio.  In our case, the system is already doing this, so this
// is basically a no-op.  [[[WDW - if you do a drain, you block, so
// don't do that.  In addition the typical SADA notion of flush is
// currently handled by wave_close.  I think this is most likely just
// terminology conflict between eSpeak and SADA.]]]
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
//
//<wave_flush

void wave_flush(void* theHandler)
{
  ENTER("wave_flush");
  //ioctl((int) theHandler, AUDIO_DRAIN, 0);
  SHOW_TIME("wave_flush > LEAVE");
}

//>
// wave_set_callback_is_output_enabled
//
// DESCRIPTION:
//
// Sets the callback to call from wave_write before it sends data to
// be played.  It helps wave_write determine if the data should be
// thrown away or not.
//
// PARAMETERS:
//
// cb: the callback to call from wave_write
//
//<wave_set_callback_is_output_enabled

void wave_set_callback_is_output_enabled(t_wave_callback* cb)
{
  my_callback_is_output_enabled = cb;
}

//>
// wave_test_get_write_buffer
//
// DESCRIPTION:
//
// Unnecessary and is used for debug output from
// speak_lib.cpp:dispatch_audio.
//
// RETURNS:
//
// NULL
//
//<wave_test_get_write_buffer

void *wave_test_get_write_buffer()
{
  return NULL;
}

//> 
// wave_get_read_position
//
// DESCRIPTION:
//
// Concerns the sample which is currently played by the audio layer,
// where 'sample' is a small buffer of synthesized wave data,
// identified so that the user callback could be called when the
// 'sample' is really played. The identifier is returned by
// wave_get_write_position.  This method is unused.
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
//
// RETURNS:
//
// The total number of 16-bit samples played by the audio system
// so far.
//
//<wave_get_read_position

uint32_t wave_get_read_position(void* theHandler)
{
  audio_info_t ainfo;
  ENTER("wave_get_read_position");
  ioctl((int) theHandler, AUDIO_GETINFO, &ainfo);
  SHOW("wave_get_read_position: %d\n", ainfo.play.samples);
  SHOW_TIME("wave_get_read_position > LEAVE");
  return ainfo.play.samples;
}

//>
// wave_get_write_position
//
// DESCRIPTION:
//
// Returns an identifier for a new sample, where 'sample' is a small
// buffer of synthesized wave data, identified so that the user
// callback could be called when the 'sample' is really played.  This
// implementation views the audio as one long continuous stream of
// 16-bit samples.
//
// PARAMETERS:
//
// theHandler: the audio device file descriptor
//
// GLOBALS USED/MODIFIED:
//
// total_samples_sent: used as the return value
//
// RETURNS:
//
// total_samples_sent, which is the index for the end of this long
// continuous stream.  [[[WDW: with a unit32_t managing 16-bit
// samples at 22050Hz, we have about 54 hours of play time before
// the index wraps back to 0.  We don't handle that wrapping, so
// the behavior after 54 hours of play time is undefined.]]]
//
//<wave_get_write_position

uint32_t wave_get_write_position(void* theHandler)
{
  ENTER("wave_get_write_position");
  SHOW("wave_get_write_position: %d\n", total_samples_sent);
  SHOW_TIME("wave_get_write_position > LEAVE");
  return total_samples_sent;
}

//>
// wave_get_remaining_time
//
// DESCRIPTION:
//
// Returns the remaining time (in ms) before the sample is played.
// The sample in this case is a return value from a previous call to
// wave_get_write_position.
//
// PARAMETERS:
//
// sample: an index returned from wave_get_write_position representing
//         an index into the long continuous stream of 16-bit samples
// time: a return value representing the delay in milliseconds until
//       sample is played.  A value of 0 means the sample is either
//       currently being played or it has already been played.
//
// GLOBALS USED/MODIFIED:
//
// sun_audio_fd: used to determine total number of samples played by
//               the audio system
// total_samples_skipped: used in remaining time calculation
//
// RETURNS:
//
// Time in milliseconds before the sample is played or 0 if the sample
// is currently playing or has already been played.
//
//<wave_get_remaining_time

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
  uint32_t a_time=0;
  uint32_t actual_index;

  audio_info_t ainfo;
  ENTER("wave_get_remaining_time");
  if (!time) {
    return(-1);
    SHOW_TIME("wave_get_remaining_time > LEAVE");
  }

  ioctl(sun_audio_fd, AUDIO_GETINFO, &ainfo);

  // See if this sample has already been played or is currently 
  // playing.
  //
  actual_index = sample - total_samples_skipped;
  if ((sample < total_samples_skipped) ||
      (actual_index <= ainfo.play.samples)) { 
    *time = 0;
  } else {
    a_time = ((actual_index - ainfo.play.samples) * 1000) / wave_samplerate;
    *time = (uint32_t) a_time;
  }
  SHOW("wave_get_remaining_time for %d: %d\n", sample, *time);
  SHOW_TIME("wave_get_remaining_time > LEAVE");
  return 0;
}

#else
// notdef USE_SADA

init wave_init() {return 1;}
void* wave_open(const char* the_api) {return (void *)1;}
size_t wave_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize) {return theSize;}
int wave_close(void* theHandler) {return 0;}
int wave_is_busy(void* theHandler) {return 0;}
void wave_terminate() {}
uint32_t wave_get_read_position(void* theHandler) {return 0;}
uint32_t wave_get_write_position(void* theHandler) {return 0;}
void wave_flush(void* theHandler) {}
typedef int (t_wave_callback)(void);
void wave_set_callback_is_output_enabled(t_wave_callback* cb) {}
extern void* wave_test_get_write_buffer() {return NULL;}

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
	if (!time) return(-1);
	*time = (uint32_t)0;
	return 0;
}

#endif  // of USE_PORTAUDIO

//>
//<clock_gettime2, add_time_in_ms

void clock_gettime2(struct timespec *ts)
{
  struct timeval tv;

  if (!ts)
    {
      return;
    }

  assert (gettimeofday(&tv, NULL) != -1);
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = tv.tv_usec*1000;
}

void add_time_in_ms(struct timespec *ts, int time_in_ms)
{
  if (!ts)
    {
      return;
    }

  uint64_t t_ns = (uint64_t)ts->tv_nsec + 1000000 * (uint64_t)time_in_ms;
  while(t_ns >= ONE_BILLION)
    {
      SHOW("event > add_time_in_ms ns: %d sec %Lu nsec \n", ts->tv_sec, t_ns);
      ts->tv_sec += 1;
      t_ns -= ONE_BILLION;	  
    }
  ts->tv_nsec = (long int)t_ns;
}

#endif   // USE_ASYNC

//>
