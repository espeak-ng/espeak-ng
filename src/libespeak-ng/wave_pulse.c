/*
 * Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>
 * Copyright (C) 2015-2016 Reece H. Dunn
 * eSpeak driver for PulseAudio
 * based on the XMMS PulseAudio Plugin
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

// TBD:
// * ARCH_BIG
// * uint64? a_timing_info.read_index
// * prebuf,... size?
// * 0.9.6: pb pulse_free using tlength=8820 (max size never returned -> tlength=10000 ok, but higher drain).
//
#include "config.h"

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <pulse/pulseaudio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <espeak-ng/espeak_ng.h>

#include "speech.h"
#include "wave.h"

enum {
	/* return value */
	PULSE_OK = 0,
	PULSE_ERROR = -1,
	PULSE_NO_CONNECTION = -2
};

#ifdef USE_PULSEAUDIO

static t_wave_callback *my_callback_is_output_enabled = NULL;

#define SAMPLE_RATE 22050
#define ESPEAK_FORMAT PA_SAMPLE_S16LE
#define ESPEAK_CHANNEL 1

#define MAXLENGTH 132300
#define TLENGTH 4410
#define PREBUF 2200
#define MINREQ 880

#ifdef USE_PORTAUDIO
// rename functions to be wrapped
#define wave_open wave_pulse_open
#define wave_write wave_pulse_write
#define wave_close wave_pulse_close
#define wave_is_busy wave_pulse_is_busy
#define wave_terminate wave_pulse_terminate
#define wave_get_read_position wave_pulse_get_read_position
#define wave_get_write_position wave_pulse_get_write_position
#define wave_flush wave_pulse_flush
#define wave_set_callback_is_output_enabled wave_pulse_set_callback_is_output_enabled
#define wave_get_remaining_time wave_pulse_get_remaining_time

// check whether we can connect to PulseAudio
#include <pulse/simple.h>
int is_pulse_running()
{
	pa_sample_spec ss;
	ss.format = ESPEAK_FORMAT;
	ss.rate = SAMPLE_RATE;
	ss.channels = ESPEAK_CHANNEL;

	pa_simple *s = pa_simple_new(NULL, "eSpeak", PA_STREAM_PLAYBACK, NULL, "is_pulse_running", &ss, NULL, NULL, NULL);
	if (s) {
		pa_simple_free(s);
		return 1;
	} else
		return 0;
}

#endif

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
				fprintf(stderr, "Connection died: %s\n", context ? pa_strerror(pa_context_errno(context)) : "NULL"); \
			goto label; \
		}  \
} while (0);

#define CHECK_CONNECTED(retval) \
	do { \
		if (!connected) return retval; \
	} while (0);

#define CHECK_CONNECTED_NO_RETVAL(id)                   \
	do {                                  \
		if (!connected) { return; } \
	} while (0);

static void subscribe_cb(struct pa_context *c, enum pa_subscription_event_type t, uint32_t index, void *userdata)
{
	(void)userdata; // unused

	assert(c);

	if (!stream ||
	    index != pa_stream_get_index(stream) ||
	    (t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT|PA_SUBSCRIPTION_EVENT_CHANGE) &&
	     t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT|PA_SUBSCRIPTION_EVENT_NEW)))
		return;
}

static void context_state_cb(pa_context *c, void *userdata)
{
	(void)userdata; // unused

	assert(c);

	switch (pa_context_get_state(c))
	{
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

static void stream_state_cb(pa_stream *s, void *userdata)
{
	(void)userdata; // unused

	assert(s);

	switch (pa_stream_get_state(s))
	{
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

static void stream_success_cb(pa_stream *s, int success, void *userdata)
{
	assert(s);

	if (userdata)
		*(int *)userdata = success;

	pa_threaded_mainloop_signal(mainloop, 0);
}

static void context_success_cb(pa_context *c, int success, void *userdata)
{
	assert(c);

	if (userdata)
		*(int *)userdata = success;

	pa_threaded_mainloop_signal(mainloop, 0);
}

static void stream_request_cb(pa_stream *s, size_t length, void *userdata)
{
	(void)length; // unused
	(void)userdata; // unused

	assert(s);

	pa_threaded_mainloop_signal(mainloop, 0);
}

static void stream_latency_update_cb(pa_stream *s, void *userdata)
{
	(void)userdata; // unused

	assert(s);

	pa_threaded_mainloop_signal(mainloop, 0);
}

static int pulse_free(void)
{
	size_t l = 0;
	pa_operation *o = NULL;

	CHECK_CONNECTED(0);

	pa_threaded_mainloop_lock(mainloop);
	CHECK_DEAD_GOTO(fail, 1);

	if ((l = pa_stream_writable_size(stream)) == (size_t)-1) {
		fprintf(stderr, "pa_stream_writable_size() failed: %s", pa_strerror(pa_context_errno(context)));
		l = 0;
		goto fail;
	}

	/* If this function is called twice with no pulse_write() call in
	 * between this means we should trigger the playback */
	if (do_trigger) {
		int success = 0;

		if (!(o = pa_stream_trigger(stream, stream_success_cb, &success))) {
			fprintf(stderr, "pa_stream_trigger() failed: %s", pa_strerror(pa_context_errno(context)));
			goto fail;
		}

		while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
			CHECK_DEAD_GOTO(fail, 1);
			pa_threaded_mainloop_wait(mainloop);
		}

		if (!success)
			fprintf(stderr, "pa_stream_trigger() failed: %s", pa_strerror(pa_context_errno(context)));
	}

fail:
	if (o)
		pa_operation_unref(o);

	pa_threaded_mainloop_unlock(mainloop);

	do_trigger = !!l;
	return (int)l;
}

static int pulse_playing(const pa_timing_info *the_timing_info)
{
	int r = 0;
	const pa_timing_info *i;

	assert(the_timing_info);

	CHECK_CONNECTED(0);

	pa_threaded_mainloop_lock(mainloop);

	for (;;) {
		CHECK_DEAD_GOTO(fail, 1);

		if ((i = pa_stream_get_timing_info(stream)))
			break;
		if (pa_context_errno(context) != PA_ERR_NODATA) {
			fprintf(stderr, "pa_stream_get_timing_info() failed: %s", pa_strerror(pa_context_errno(context)));
			goto fail;
		}

		pa_threaded_mainloop_wait(mainloop);
	}

	r = i->playing;
	memcpy((void *)the_timing_info, (void *)i, sizeof(pa_timing_info));

fail:
	pa_threaded_mainloop_unlock(mainloop);

	return r;
}

static void pulse_write(void *ptr, int length)
{
	CHECK_CONNECTED_NO_RETVAL();

	pa_threaded_mainloop_lock(mainloop);
	CHECK_DEAD_GOTO(fail, 1);

	if (pa_stream_write(stream, ptr, length, NULL, PA_SEEK_RELATIVE, (pa_seek_mode_t)0) < 0) {
		fprintf(stderr, "pa_stream_write() failed: %s", pa_strerror(pa_context_errno(context)));
		goto fail;
	}

	do_trigger = 0;
	written += length;
fail:
	pa_threaded_mainloop_unlock(mainloop);
}

static int drain(void)
{
	pa_operation *o = NULL;
	int success = 0;
	int ret = PULSE_ERROR;

	CHECK_CONNECTED(ret);

	pa_threaded_mainloop_lock(mainloop);
	CHECK_DEAD_GOTO(fail, 0);

	if (!(o = pa_stream_drain(stream, stream_success_cb, &success))) {
		fprintf(stderr, "pa_stream_drain() failed: %s\n", pa_strerror(pa_context_errno(context)));
		goto fail;
	}

	while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
		CHECK_DEAD_GOTO(fail, 1);
		pa_threaded_mainloop_wait(mainloop);
	}

	if (!success)
		fprintf(stderr, "pa_stream_drain() failed: %s\n", pa_strerror(pa_context_errno(context)));
	else
		ret = PULSE_OK;
fail:
	if (o)
		pa_operation_unref(o);

	pa_threaded_mainloop_unlock(mainloop);

	return ret;
}

static void pulse_close(void)
{
	drain();

	connected = 0;

	if (mainloop)
		pa_threaded_mainloop_stop(mainloop);

	connected = 0;

	if (context) {
		pa_context_disconnect(context);
		pa_context_unref(context);
		context = NULL;
	}

	if (mainloop) {
		pa_threaded_mainloop_free(mainloop);
		mainloop = NULL;
	}
}

static int pulse_open(const char *device)
{
	pa_sample_spec ss;
	pa_operation *o = NULL;
	int success;
	int ret = PULSE_ERROR;

	assert(!mainloop);
	assert(!context);
	assert(!stream);
	assert(!connected);

	pthread_mutex_init(&pulse_mutex, (const pthread_mutexattr_t *)NULL);

	ss.format = ESPEAK_FORMAT;
	ss.rate = wave_samplerate;
	ss.channels = ESPEAK_CHANNEL;

	if (!pa_sample_spec_valid(&ss))
		return false;

	if (!(mainloop = pa_threaded_mainloop_new()))
		goto fail;

	pa_threaded_mainloop_lock(mainloop);

	if (!(context = pa_context_new(pa_threaded_mainloop_get_api(mainloop), "eSpeak")))
		goto unlock_and_fail;

	pa_context_set_state_callback(context, context_state_cb, NULL);
	pa_context_set_subscribe_callback(context, subscribe_cb, NULL);

	if (pa_context_connect(context, NULL, (pa_context_flags_t)0, NULL) < 0) {
		fprintf(stderr, "Failed to connect to server: %s", pa_strerror(pa_context_errno(context)));
		ret = PULSE_NO_CONNECTION;
		goto unlock_and_fail;
	}

	if (pa_threaded_mainloop_start(mainloop) < 0)
		goto unlock_and_fail;

	// Wait until the context is ready
	pa_threaded_mainloop_wait(mainloop);

	if (pa_context_get_state(context) != PA_CONTEXT_READY) {
		fprintf(stderr, "Failed to connect to server: %s", pa_strerror(pa_context_errno(context)));
		ret = PULSE_NO_CONNECTION;
		if (mainloop)
			pa_threaded_mainloop_stop(mainloop);
		goto unlock_and_fail;
	}

	if (!(stream = pa_stream_new(context, "unknown", &ss, NULL))) {
		fprintf(stderr, "Failed to create stream: %s", pa_strerror(pa_context_errno(context)));
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

	if (pa_stream_connect_playback(stream, device, &a_attr, (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING|PA_STREAM_AUTO_TIMING_UPDATE), NULL, NULL) < 0) {
		fprintf(stderr, "Failed to connect stream: %s", pa_strerror(pa_context_errno(context)));
		goto unlock_and_fail;
	}

	// Wait until the stream is ready
	pa_threaded_mainloop_wait(mainloop);

	if (pa_stream_get_state(stream) != PA_STREAM_READY) {
		fprintf(stderr, "Failed to connect stream: %s", pa_strerror(pa_context_errno(context)));
		goto unlock_and_fail;
	}

	// Now subscribe to events
	if (!(o = pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK_INPUT, context_success_cb, &success))) {
		fprintf(stderr, "pa_context_subscribe() failed: %s", pa_strerror(pa_context_errno(context)));
		goto unlock_and_fail;
	}

	while (pa_operation_get_state(o) != PA_OPERATION_DONE) {
		CHECK_DEAD_GOTO(fail, 1);
		pa_threaded_mainloop_wait(mainloop);
	}

	pa_operation_unref(o);

	do_trigger = 0;
	written = 0;
	time_offset_msec = 0;
	just_flushed = 0;
	connected = 1;

	pa_threaded_mainloop_unlock(mainloop);

	return PULSE_OK;
unlock_and_fail:
	pa_threaded_mainloop_unlock(mainloop);
fail:
	if (ret == PULSE_NO_CONNECTION) {
		if (context) {
			pa_context_disconnect(context);
			pa_context_unref(context);
			context = NULL;
		}

		if (mainloop) {
			pa_threaded_mainloop_free(mainloop);
			mainloop = NULL;
		}
	} else
		pulse_close();

	return ret;
}

void wave_flush(void *theHandler)
{
	(void)theHandler; // unused
}

void wave_set_callback_is_output_enabled(t_wave_callback *cb)
{
	my_callback_is_output_enabled = cb;
}

void *wave_open(int srate, const char *device)
{
	stream = NULL;
	wave_samplerate = srate;

	if (pulse_open(device) != PULSE_OK)
		return NULL;

	return (void *)1;
}

size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize)
{
	(void)theHandler; // unused

	size_t bytes_to_write = theSize;
	char *aBuffer = theMono16BitsWaveBuffer;

	assert(stream);

	size_t aTotalFreeMem = 0;

	pthread_mutex_lock(&pulse_mutex);

	while (1) {
		if (my_callback_is_output_enabled
		    && (0 == my_callback_is_output_enabled())) {
			theSize = 0;
			goto terminate;
		}

		aTotalFreeMem = pulse_free();
		if (aTotalFreeMem >= bytes_to_write)
			break;

		// TBD: check if really helpful
		if (aTotalFreeMem >= MAXLENGTH*2)
			aTotalFreeMem = MAXLENGTH*2;

		// 500: threshold for avoiding too many calls to pulse_write
		if (aTotalFreeMem > 500) {
			pulse_write(aBuffer, aTotalFreeMem);
			bytes_to_write -= aTotalFreeMem;
			aBuffer += aTotalFreeMem;
		}

		usleep(10000);
	}

	pulse_write(aBuffer, bytes_to_write);
terminate:
	pthread_mutex_unlock(&pulse_mutex);
	return theSize;
}

int wave_close(void *theHandler)
{
	(void)theHandler; // unused

	static int aStopStreamCount = 0;

	// Avoid race condition by making sure this function only
	// gets called once at a time
	aStopStreamCount++;
	if (aStopStreamCount != 1)
		return 0;

	int a_status = pthread_mutex_lock(&pulse_mutex);
	if (a_status) {
		aStopStreamCount = 0; // last action
		return PULSE_ERROR;
	}

	drain();

	pthread_mutex_unlock(&pulse_mutex);

	aStopStreamCount = 0; // last action
	return PULSE_OK;
}

int wave_is_busy(void *theHandler)
{
	(void)theHandler; // unused

	pa_timing_info a_timing_info = {0};
	int active = pulse_playing(&a_timing_info);
	return active;
}

void wave_terminate()
{
	pthread_mutex_t *a_mutex = NULL;
	a_mutex = &pulse_mutex;
	pthread_mutex_lock(a_mutex);

	pulse_close();

	pthread_mutex_unlock(a_mutex);
	pthread_mutex_destroy(a_mutex);
}

uint32_t wave_get_read_position(void *theHandler)
{
	(void)theHandler; // unused

	pa_timing_info a_timing_info = {0};
	pulse_playing(&a_timing_info);
	return a_timing_info.read_index;
}

uint32_t wave_get_write_position(void *theHandler)
{
	(void)theHandler; // unused

	pa_timing_info a_timing_info = {0};
	pulse_playing(&a_timing_info);
	return a_timing_info.write_index;
}

int wave_get_remaining_time(uint32_t sample, uint32_t *time)
{
	double a_time = 0;

	if (!time || !stream)
		return -1;

	pa_timing_info a_timing_info = {0};
	pulse_playing(&a_timing_info);

	if (sample > a_timing_info.read_index) {
		// TBD: take in account time suplied by portaudio V18 API
		a_time = sample - a_timing_info.read_index;
		a_time = 0.5 + (a_time * 1000.0) / wave_samplerate;
	} else
		a_time = 0;

	*time = (uint32_t)a_time;

	return 0;
}

#endif
