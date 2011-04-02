/***************************************************************************
 *   Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>                  *
 *   eSpeak driver for PulseAudio                                          *
 *   based on the XMMS PulseAudio Plugin                                   *
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
// TBD:
// * ARCH_BIG
// * uint64? a_timing_info.read_index
// * prebuf,... size?
// * 0.9.6: pb pulse_free using tlength=8820 (max size never returned -> tlength=10000 ok, but higher drain).
//
#include "speech.h"

#ifdef USE_ASYNC
// This source file is only used for asynchronious modes

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <pulse/pulseaudio.h>
#include <pthread.h>

#ifndef PLATFORM_WINDOWS
#include <unistd.h>
#endif
#include "wave.h"
#include "debug.h"

//<Definitions

enum {ONE_BILLION=1000000000};

enum {
//   /* 100ms. 
//      If a greater value is set (several seconds), 
//      please update _pulse_timeout_start accordingly */
//   PULSE_TIMEOUT_IN_USEC = 100000,  

  /* return value */
  PULSE_OK = 0,
  PULSE_ERROR = -1,
  PULSE_NO_CONNECTION = -2
};

#ifdef USE_PULSEAUDIO

static t_wave_callback* my_callback_is_output_enabled=NULL;

#define ESPEAK_FORMAT PA_SAMPLE_S16LE
#define ESPEAK_CHANNEL 1

#define MAXLENGTH 132300
#define TLENGTH 4410
#define PREBUF 2200
#define MINREQ 880
#define FRAGSIZE 0

static pthread_mutex_t pulse_mutex;

static pa_context *context = NULL;
static pa_stream *stream = NULL;
static pa_threaded_mainloop *mainloop = NULL;

static int do_trigger = 0;
static uint64_t written = 0;
static int time_offset_msec = 0;
static int just_flushed = 0;

static int connected = 0;
static int wave_samplerate;

#define CHECK_DEAD_GOTO(label, warn) do { \
if (!mainloop || \
    !context || pa_context_get_state(context) != PA_CONTEXT_READY || \
    !stream || pa_stream_get_state(stream) != PA_STREAM_READY) { \
        if (warn) \
            SHOW("Connection died: %s\n", context ? pa_strerror(pa_context_errno(context)) : "NULL"); \
        goto label; \
    }  \
} while(0);

#define CHECK_CONNECTED(retval) \
do { \
    if (!connected) return retval; \
} while (0);

#define CHECK_CONNECTED_NO_RETVAL(id)					\
  do {									\
    if (!connected){ SHOW("CHECK_CONNECTED_NO_RETVAL: !pulse_connected\n", ""); return;	} \
  } while (0);

//>


// static void display_timing_info(const pa_timing_info* the_time)
// {
//   const struct timeval *tv=&(the_time->timestamp);

//   SHOW_TIME("ti>");
//   SHOW("ti> timestamp=%03d.%03dms\n",(int)(tv->tv_sec%1000), (int)(tv->tv_usec/1000));
//   SHOW("ti> synchronized_clocks=%d\n",the_time->synchronized_clocks);
//   SHOW("ti> sink_usec=%ld\n",the_time->sink_usec);
//   SHOW("ti> source_usec=%ld\n",the_time->source_usec);
//   SHOW("ti> transport=%ld\n",the_time->transport_usec);
//   SHOW("ti> playing=%d\n",the_time->playing);
//   SHOW("ti> write_index_corrupt=%d\n",the_time->write_index_corrupt);
//   SHOW("ti> write_index=0x%lx\n",the_time->write_index);
//   SHOW("ti> read_index_corrupt=%d\n",the_time->read_index_corrupt);
//   SHOW("ti> read_index=0x%lx\n",the_time->read_index);
// }

static void subscribe_cb(struct pa_context *c, enum pa_subscription_event_type t, uint32_t index, void *userdata) {
  ENTER(__FUNCTION__);
    
    assert(c);

    if (!stream ||
        index != pa_stream_get_index(stream) ||
        (t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT|PA_SUBSCRIPTION_EVENT_CHANGE) &&
         t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT|PA_SUBSCRIPTION_EVENT_NEW)))
        return;
}

static void context_state_cb(pa_context *c, void *userdata) {
  ENTER(__FUNCTION__);
    assert(c);

    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
            pa_threaded_mainloop_signal(mainloop, 0);
            break;

        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;
    }
}

static void stream_state_cb(pa_stream *s, void * userdata) {
  ENTER(__FUNCTION__);
    assert(s);

    switch (pa_stream_get_state(s)) {

        case PA_STREAM_READY:
        case PA_STREAM_FAILED:
        case PA_STREAM_TERMINATED:
            pa_threaded_mainloop_signal(mainloop, 0);
            break;

        case PA_STREAM_UNCONNECTED:
        case PA_STREAM_CREATING:
            break;
    }
}

static void stream_success_cb(pa_stream *s, int success, void *userdata) {
  ENTER(__FUNCTION__);
    assert(s);

    if (userdata)
        *(int*) userdata = success;
    
    pa_threaded_mainloop_signal(mainloop, 0);
}

static void context_success_cb(pa_context *c, int success, void *userdata) {
  ENTER(__FUNCTION__);
    assert(c);

    if (userdata)
        *(int*) userdata = success;
    
    pa_threaded_mainloop_signal(mainloop, 0);
}

static void stream_request_cb(pa_stream *s, size_t length, void *userdata) {
  ENTER(__FUNCTION__);
    assert(s);

    pa_threaded_mainloop_signal(mainloop, 0);
}

static void stream_latency_update_cb(pa_stream *s, void *userdata) {
  //  ENTER(__FUNCTION__);
    assert(s);

    pa_threaded_mainloop_signal(mainloop, 0);
}

static int pulse_free(void) {
  ENTER(__FUNCTION__);
    size_t l = 0;
    pa_operation *o = NULL;

    CHECK_CONNECTED(0);

    SHOW("pulse_free: %s (call)\n", "pa_threaded_main_loop_lock");
    pa_threaded_mainloop_lock(mainloop);
    CHECK_DEAD_GOTO(fail, 1);

    if ((l = pa_stream_writable_size(stream)) == (size_t) -1) {
        SHOW("pa_stream_writable_size() failed: %s", pa_strerror(pa_context_errno(context)));
        l = 0;
        goto fail;
    }

    SHOW("pulse_free: %s (ret=%d)\n", "pa_stream_writable_size", l);

    /* If this function is called twice with no pulse_write() call in
     * between this means we should trigger the playback */
    if (do_trigger) {
        int success = 0;

	SHOW("pulse_free: %s (call)\n", "pa_stream_trigger");
        if (!(o = pa_stream_trigger(stream, stream_success_cb, &success))) {
            SHOW("pa_stream_trigger() failed: %s", pa_strerror(pa_context_errno(context)));
            goto fail;
        }
        
	SHOW("pulse_free: %s (call)\n", "pa_threaded_main_loop");
        while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
            CHECK_DEAD_GOTO(fail, 1);
            pa_threaded_mainloop_wait(mainloop);
        } 
	SHOW("pulse_free: %s (ret)\n", "pa_threaded_main_loop");
       
        if (!success)
            SHOW("pa_stream_trigger() failed: %s", pa_strerror(pa_context_errno(context)));
    }
    
fail:
    SHOW("pulse_free: %s (call)\n", "pa_operation_unref");
    if (o)
        pa_operation_unref(o);
    
    SHOW("pulse_free: %s (call)\n", "pa_threaded_main_loop_unlock");
    pa_threaded_mainloop_unlock(mainloop);

    do_trigger = !!l;
    SHOW("pulse_free: %d (ret)\n", (int)l);
    return (int) l;
}

static int pulse_playing(const pa_timing_info *the_timing_info) {
  ENTER(__FUNCTION__);
    int r = 0;
    const pa_timing_info *i;

    assert(the_timing_info);

    CHECK_CONNECTED(0);
    
    pa_threaded_mainloop_lock(mainloop);

    for (;;) {
        CHECK_DEAD_GOTO(fail, 1);

        if ((i = pa_stream_get_timing_info(stream)))
	  {
            break;        
	  }
        if (pa_context_errno(context) != PA_ERR_NODATA) {
            SHOW("pa_stream_get_timing_info() failed: %s", pa_strerror(pa_context_errno(context)));
            goto fail;
        }

        pa_threaded_mainloop_wait(mainloop);
    }

    r = i->playing;
    memcpy((void*)the_timing_info, (void*)i, sizeof(pa_timing_info));

    //    display_timing_info(i);

fail:
    pa_threaded_mainloop_unlock(mainloop);

    return r;
}


// static void pulse_flush(int time) {
//   ENTER(__FUNCTION__);
//     pa_operation *o = NULL;
//     int success = 0;

//     CHECK_CONNECTED();

//     pa_threaded_mainloop_lock(mainloop);
//     CHECK_DEAD_GOTO(fail, 1);

//     if (!(o = pa_stream_flush(stream, stream_success_cb, &success))) {
//         SHOW("pa_stream_flush() failed: %s", pa_strerror(pa_context_errno(context)));
//         goto fail;
//     }
    
//     while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
//         CHECK_DEAD_GOTO(fail, 1);
//         pa_threaded_mainloop_wait(mainloop);
//     }

//     if (!success)
//         SHOW("pa_stream_flush() failed: %s", pa_strerror(pa_context_errno(context)));
    
//     written = (uint64_t) (((double) time * pa_bytes_per_second(pa_stream_get_sample_spec(stream))) / 1000);
//     just_flushed = 1;
//     time_offset_msec = time;
    
// fail:
//     if (o)
//         pa_operation_unref(o);
    
//     pa_threaded_mainloop_unlock(mainloop);
// }


static void pulse_write(void* ptr, int length) {
  ENTER(__FUNCTION__);


  SHOW("pulse_write > length=%d\n", length);

    CHECK_CONNECTED_NO_RETVAL();

    pa_threaded_mainloop_lock(mainloop);
    CHECK_DEAD_GOTO(fail, 1);

    if (pa_stream_write(stream, ptr, length, NULL, PA_SEEK_RELATIVE, (pa_seek_mode_t)0) < 0) {
        SHOW("pa_stream_write() failed: %s", pa_strerror(pa_context_errno(context)));
        goto fail;
    }
    
    do_trigger = 0;
    written += length;

fail:
    
    pa_threaded_mainloop_unlock(mainloop);
}

static int drain(void) {
    pa_operation *o = NULL;
    int success = 0;
    int ret = PULSE_ERROR;

    ENTER(__FUNCTION__);

    CHECK_CONNECTED(ret);

    pa_threaded_mainloop_lock(mainloop);
    CHECK_DEAD_GOTO(fail, 0);

    SHOW_TIME("pa_stream_drain (call)");
    if (!(o = pa_stream_drain(stream, stream_success_cb, &success))) {
        SHOW("pa_stream_drain() failed: %s\n", pa_strerror(pa_context_errno(context)));
        goto fail;
    }
    
    SHOW_TIME("pa_threaded_mainloop_wait (call)");
    while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
        CHECK_DEAD_GOTO(fail, 1);
        pa_threaded_mainloop_wait(mainloop);
    }
    SHOW_TIME("pa_threaded_mainloop_wait (ret)");

    if (!success) {
      SHOW("pa_stream_drain() failed: %s\n", pa_strerror(pa_context_errno(context)));
    } 
    else {
      ret = PULSE_OK;
    }
    
fail:
    SHOW_TIME("pa_operation_unref (call)");
    if (o)
        pa_operation_unref(o);
 
    pa_threaded_mainloop_unlock(mainloop);
    SHOW_TIME("drain (ret)");
    
    return ret;
}


static void pulse_close(void) {

  ENTER(__FUNCTION__);
    
  drain();

  connected = 0;

  if (mainloop)
    pa_threaded_mainloop_stop(mainloop);

    connected = 0;

  if (context) {
    SHOW_TIME("pa_context_disconnect (call)");
    pa_context_disconnect(context);
    pa_context_unref(context);
    context = NULL;
  }
  
  if (mainloop) {
  SHOW_TIME("pa_threaded_mainloop_free (call)");
    pa_threaded_mainloop_free(mainloop);
    mainloop = NULL;
  }
  SHOW_TIME("pulse_close (ret)");
  
}


static int pulse_open()
{
  ENTER(__FUNCTION__);
    pa_sample_spec ss;
    pa_operation *o = NULL;
    int success;
    int ret = PULSE_ERROR;

    assert(!mainloop);
    assert(!context);
    assert(!stream);
    assert(!connected);

    pthread_mutex_init( &pulse_mutex, (const pthread_mutexattr_t *)NULL);

    ss.format = ESPEAK_FORMAT;
    ss.rate = wave_samplerate;
    ss.channels = ESPEAK_CHANNEL;

    if (!pa_sample_spec_valid(&ss))
      return false;

    SHOW_TIME("pa_threaded_mainloop_new (call)");
    if (!(mainloop = pa_threaded_mainloop_new())) {
      SHOW("Failed to allocate main loop\n","");
        goto fail;
    }

    pa_threaded_mainloop_lock(mainloop);

    SHOW_TIME("pa_context_new (call)");
    if (!(context = pa_context_new(pa_threaded_mainloop_get_api(mainloop), "eSpeak"))) {
      SHOW("Failed to allocate context\n","");
      goto unlock_and_fail;
    }

    pa_context_set_state_callback(context, context_state_cb, NULL);
    pa_context_set_subscribe_callback(context, subscribe_cb, NULL);

    SHOW_TIME("pa_context_connect (call)");
    if (pa_context_connect(context, NULL, (pa_context_flags_t)0, NULL) < 0) {
        SHOW("Failed to connect to server: %s", pa_strerror(pa_context_errno(context)));
	ret = PULSE_NO_CONNECTION;
        goto unlock_and_fail;
    }

    SHOW_TIME("pa_threaded_mainloop_start (call)");
    if (pa_threaded_mainloop_start(mainloop) < 0) {
      SHOW("Failed to start main loop","");
        goto unlock_and_fail;
    }

    /* Wait until the context is ready */
    SHOW_TIME("pa_threaded_mainloop_wait");
    pa_threaded_mainloop_wait(mainloop);

    if (pa_context_get_state(context) != PA_CONTEXT_READY) {
        SHOW("Failed to connect to server: %s", pa_strerror(pa_context_errno(context)));
	ret = PULSE_NO_CONNECTION;
 	if (mainloop)
 	  pa_threaded_mainloop_stop(mainloop);
        goto unlock_and_fail;
    }

    SHOW_TIME("pa_stream_new");
    if (!(stream = pa_stream_new(context, "unknown", &ss, NULL))) {
        SHOW("Failed to create stream: %s", pa_strerror(pa_context_errno(context)));
        goto unlock_and_fail;
    }

    pa_stream_set_state_callback(stream, stream_state_cb, NULL);
    pa_stream_set_write_callback(stream, stream_request_cb, NULL);
    pa_stream_set_latency_update_callback(stream, stream_latency_update_cb, NULL);

    pa_buffer_attr a_attr;

    a_attr.maxlength = MAXLENGTH;
    a_attr.tlength = TLENGTH;
    a_attr.prebuf = PREBUF;
    a_attr.minreq = MINREQ;
    a_attr.fragsize = 0;

    SHOW_TIME("pa_connect_playback");
    if (pa_stream_connect_playback(stream, NULL, &a_attr, (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING|PA_STREAM_AUTO_TIMING_UPDATE), NULL, NULL) < 0) {
        SHOW("Failed to connect stream: %s", pa_strerror(pa_context_errno(context)));
        goto unlock_and_fail;
    }

    /* Wait until the stream is ready */
    SHOW_TIME("pa_threaded_mainloop_wait");
    pa_threaded_mainloop_wait(mainloop);

    if (pa_stream_get_state(stream) != PA_STREAM_READY) {
        SHOW("Failed to connect stream: %s", pa_strerror(pa_context_errno(context)));
        goto unlock_and_fail;
    }

    /* Now subscribe to events */
    SHOW_TIME("pa_context_subscribe");
    if (!(o = pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK_INPUT, context_success_cb, &success))) {
        SHOW("pa_context_subscribe() failed: %s", pa_strerror(pa_context_errno(context)));
        goto unlock_and_fail;
    }
    
    success = 0;
    SHOW_TIME("pa_threaded_mainloop_wait");
    while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
        CHECK_DEAD_GOTO(fail, 1);
        pa_threaded_mainloop_wait(mainloop);
    }

    pa_operation_unref(o);

    if (!success) {
        SHOW("pa_context_subscribe() failed: %s", pa_strerror(pa_context_errno(context)));
        goto unlock_and_fail;
    }

    do_trigger = 0;
    written = 0;
    time_offset_msec = 0;
    just_flushed = 0;
    connected = 1;
    
    pa_threaded_mainloop_unlock(mainloop);
    SHOW_TIME("pulse_open (ret true)");
   
    return PULSE_OK;

unlock_and_fail:

    if (o)
        pa_operation_unref(o);
    
    pa_threaded_mainloop_unlock(mainloop);
    
fail:

    //    pulse_close();

  if (ret == PULSE_NO_CONNECTION) {
    if (context) {
      SHOW_TIME("pa_context_disconnect (call)");
      pa_context_disconnect(context);
      pa_context_unref(context);
      context = NULL;
    }
  
    if (mainloop) {
      SHOW_TIME("pa_threaded_mainloop_free (call)");
      pa_threaded_mainloop_free(mainloop);
      mainloop = NULL;
    }  
  } 
  else {
    pulse_close();
  }

  SHOW_TIME("pulse_open (ret false)");
  
  return ret;

}

void wave_flush(void* theHandler)
{
  ENTER("wave_flush");

//   if (my_stream_could_start)
//     {
// //       #define buf 1024
// //       static char a_buffer[buf*2];
// //       memset(a_buffer,0,buf*2);
// //       wave_write(theHandler, a_buffer, buf*2);
//       start_stream();
//     }
}



//<wave_set_callback_is_output_enabled

void wave_set_callback_is_output_enabled(t_wave_callback* cb)
{
  my_callback_is_output_enabled = cb;
}

//>
//<wave_init

void wave_init(int srate)
{
  ENTER("wave_init");

  stream = NULL;
	wave_samplerate = srate;

  pulse_open();
}

//>
//<wave_open

void* wave_open(const char* the_api)
{
  ENTER("wave_open");
  return((void*)1);
}

//>
//<wave_write

size_t wave_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize)
{
  ENTER("wave_write");
  size_t bytes_to_write = theSize;
  char* aBuffer=theMono16BitsWaveBuffer;

  assert(stream);

  size_t aTotalFreeMem=0;

  pthread_mutex_lock(&pulse_mutex);

  while (1) 
    {
      if (my_callback_is_output_enabled 
	  && (0==my_callback_is_output_enabled()))
	{
	  SHOW_TIME("wave_write > my_callback_is_output_enabled: no!");
	  theSize=0;
	  goto terminate;
	}

      aTotalFreeMem = pulse_free();
      if (aTotalFreeMem >= bytes_to_write)
	{
	  SHOW("wave_write > aTotalFreeMem(%d) >= bytes_to_write(%d)\n", aTotalFreeMem, bytes_to_write);
	  break;
	}
 
      // TBD: check if really helpful
      if (aTotalFreeMem >= MAXLENGTH*2)
 	{
 	  aTotalFreeMem = MAXLENGTH*2;
 	}
       
      SHOW("wave_write > wait: aTotalFreeMem(%d) < bytes_to_write(%d)\n", aTotalFreeMem, bytes_to_write);

      // 500: threshold for avoiding too many calls to pulse_write
      if (aTotalFreeMem>500)
	{
	  pulse_write(aBuffer, aTotalFreeMem);
	  bytes_to_write -= aTotalFreeMem;
	  aBuffer += aTotalFreeMem;
	}

      usleep(10000);
    }

  pulse_write(aBuffer, bytes_to_write);

 terminate:
  pthread_mutex_unlock(&pulse_mutex);
  SHOW("wave_write: theSize=%d", theSize);
  SHOW_TIME("wave_write > LEAVE");
  return theSize;
}

//>
//<wave_close

int wave_close(void* theHandler)
{
  SHOW_TIME("wave_close > ENTER");

  int a_status = pthread_mutex_lock(&pulse_mutex);
  if (a_status) {
    SHOW("Error: pulse_mutex lock=%d (%s)\n", a_status, __FUNCTION__);
    return PULSE_ERROR;
  }
  
  drain();

  pthread_mutex_unlock(&pulse_mutex);
  SHOW_TIME("wave_close (ret)");

  return PULSE_OK;
}

//>
//<wave_is_busy

int wave_is_busy(void* theHandler)
{
  SHOW_TIME("wave_is_busy");

  pa_timing_info a_timing_info;
  int active = pulse_playing(&a_timing_info);
  SHOW("wave_is_busy: %d\n",active);
  return active;
}

//>
//<wave_terminate

void wave_terminate()
{
  ENTER("wave_terminate");

//   Pa_Terminate();

  int a_status;
  pthread_mutex_t* a_mutex = NULL;
  a_mutex = &pulse_mutex;
  a_status = pthread_mutex_lock(a_mutex);

  pulse_close();

  SHOW_TIME("unlock mutex");
  a_status = pthread_mutex_unlock(a_mutex);
  pthread_mutex_destroy(a_mutex);
}

//>
//<wave_get_read_position, wave_get_write_position, wave_get_remaining_time

uint32_t wave_get_read_position(void* theHandler)
{
  pa_timing_info a_timing_info;
  pulse_playing(&a_timing_info);
  SHOW("wave_get_read_position > %lx\n", a_timing_info.read_index);
  return a_timing_info.read_index;
}

uint32_t wave_get_write_position(void* theHandler)
{
  pa_timing_info a_timing_info;
  pulse_playing(&a_timing_info);
  SHOW("wave_get_read_position > %lx\n", a_timing_info.write_index);
  return a_timing_info.write_index;
}

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
  double a_time=0;

  if (!time || !stream)
    {
      SHOW("event get_remaining_time> %s\n","audio device not available");	  
      return -1;
    }

  pa_timing_info a_timing_info;
  pulse_playing(&a_timing_info);

  if (sample > a_timing_info.read_index)
    {
      // TBD: take in account time suplied by portaudio V18 API
      a_time = sample - a_timing_info.read_index;
      a_time = 0.5 + (a_time * 1000.0) / wave_samplerate;
    }
  else
    {
      a_time = 0;
    }

  SHOW("wave_get_remaining_time > sample=%d, time=%d\n", sample, (uint32_t)a_time);

  *time = (uint32_t)a_time;

  return 0;
}

//>
//<wave_test_get_write_buffer

void *wave_test_get_write_buffer()
{
  return NULL;
}


#else
// notdef USE_PULSEAUDIO


void wave_init() {}
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
