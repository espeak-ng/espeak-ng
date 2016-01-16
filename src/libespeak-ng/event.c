/*
 * Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>
 * Copyright (C) 2013-2016 Reece H. Dunn
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

// This source file is only used for asynchronious modes

#include "config.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak/speak_lib.h>

#include "speech.h"
#include "event.h"
#include "wave.h"

// my_mutex: protects my_thread_is_talking,
static pthread_mutex_t my_mutex;
static sem_t my_sem_start_is_required;
static sem_t my_sem_stop_is_required;
static sem_t my_sem_stop_is_acknowledged;
// my_thread: polls the audio duration and compares it to the duration of the first event.
static pthread_t my_thread;
static bool thread_inited;

static t_espeak_callback *my_callback = NULL;
static int my_event_is_running = 0;

enum {
	MIN_TIMEOUT_IN_MS = 10,
	ACTIVITY_TIMEOUT = 50, // in ms, check that the stream is active
	MAX_ACTIVITY_CHECK = 6
};

typedef struct t_node {
	void *data;
	struct t_node *next;
} node;

static node *head = NULL;
static node *tail = NULL;
static int node_counter = 0;
static espeak_ng_STATUS push(void *data);
static void *pop();
static void init();
static void *polling_thread(void *);

void event_set_callback(t_espeak_callback *SynthCallback)
{
	my_callback = SynthCallback;
}

void event_init(void)
{
	my_event_is_running = 0;

	// security
	pthread_mutex_init(&my_mutex, (const pthread_mutexattr_t *)NULL);
	init();

	assert(-1 != sem_init(&my_sem_start_is_required, 0, 0));
	assert(-1 != sem_init(&my_sem_stop_is_required, 0, 0));
	assert(-1 != sem_init(&my_sem_stop_is_acknowledged, 0, 0));

	pthread_attr_t a_attrib;

	if (pthread_attr_init(&a_attrib) == 0
	    && pthread_attr_setdetachstate(&a_attrib, PTHREAD_CREATE_JOINABLE) == 0) {
		thread_inited = (0 == pthread_create(&my_thread,
		                                     &a_attrib,
		                                     polling_thread,
		                                     (void *)NULL));
	}
	assert(thread_inited);
	pthread_attr_destroy(&a_attrib);
}

static espeak_EVENT *event_copy(espeak_EVENT *event)
{
	if (event == NULL)
		return NULL;

	espeak_EVENT *a_event = (espeak_EVENT *)malloc(sizeof(espeak_EVENT));
	if (a_event) {
		memcpy(a_event, event, sizeof(espeak_EVENT));

		switch (event->type)
		{
		case espeakEVENT_MARK:
		case espeakEVENT_PLAY:
			if (event->id.name)
				a_event->id.name = strdup(event->id.name);
			break;

		default:
			break;
		}
	}

	return a_event;
}

// Call the user supplied callback
//
// Note: the current sequence is:
//
// * First call with: event->type = espeakEVENT_SENTENCE
// * 0, 1 or several calls: event->type = espeakEVENT_WORD
// * Last call: event->type = espeakEVENT_MSG_TERMINATED
//

static void event_notify(espeak_EVENT *event)
{
	static unsigned int a_old_uid = 0;

	espeak_EVENT events[2];
	memcpy(&events[0], event, sizeof(espeak_EVENT));     // the event parameter in the callback function should be an array of eventd
	memcpy(&events[1], event, sizeof(espeak_EVENT));
	events[1].type = espeakEVENT_LIST_TERMINATED;           // ... terminated by an event type=0

	if (event && my_callback) {
		switch (event->type)
		{
		case espeakEVENT_SENTENCE:
			my_callback(NULL, 0, events);
			a_old_uid = event->unique_identifier;
			break;
		case espeakEVENT_MSG_TERMINATED:
		case espeakEVENT_MARK:
		case espeakEVENT_WORD:
		case espeakEVENT_END:
		case espeakEVENT_PHONEME:
		{
			if (a_old_uid != event->unique_identifier) {
				espeak_EVENT_TYPE a_new_type = events[0].type;
				events[0].type = espeakEVENT_SENTENCE;
				my_callback(NULL, 0, events);
				events[0].type = a_new_type;
				usleep(50000);
			}
			my_callback(NULL, 0, events);
			a_old_uid = event->unique_identifier;
		}
			break;
		case espeakEVENT_LIST_TERMINATED:
		case espeakEVENT_PLAY:
		default:
			break;
		}
	}
}

static int event_delete(espeak_EVENT *event)
{
	if (event == NULL)
		return 0;

	switch (event->type)
	{
	case espeakEVENT_MSG_TERMINATED:
		event_notify(event);
		break;
	case espeakEVENT_MARK:
	case espeakEVENT_PLAY:
		if (event->id.name)
			free((void *)(event->id.name));
		break;
	default:
		break;
	}

	free(event);
	return 1;
}

espeak_ng_STATUS event_declare(espeak_EVENT *event)
{
	if (!event)
		return EINVAL;

	espeak_ng_STATUS status;
	if ((status = pthread_mutex_lock(&my_mutex)) != ENS_OK) {
		sem_post(&my_sem_start_is_required);
		return status;
	}

	espeak_EVENT *a_event = event_copy(event);
	if ((status = push(a_event)) != ENS_OK) {
		event_delete(a_event);
		pthread_mutex_unlock(&my_mutex);
	} else
		status = pthread_mutex_unlock(&my_mutex);

	sem_post(&my_sem_start_is_required);

	return status;
}

espeak_ng_STATUS event_clear_all()
{
	espeak_ng_STATUS status;
	if ((status = pthread_mutex_lock(&my_mutex)) != ENS_OK)
		return status;

	int a_event_is_running = 0;
	if (my_event_is_running) {
		sem_post(&my_sem_stop_is_required);
		a_event_is_running = 1;
	} else
		init(); // clear pending events

	if ((status = pthread_mutex_unlock(&my_mutex)) != ENS_OK)
		return status;

	if (a_event_is_running) {
		while ((sem_wait(&my_sem_stop_is_acknowledged) == -1) && errno == EINTR)
			continue; // Restart when interrupted by handler
	}

	return ENS_OK;
}

static int sleep_until_timeout_or_stop_request(uint32_t time_in_ms)
{
	int a_stop_is_required = 0;
	struct timespec ts;
	struct timeval tv;
	int err = 0;

	clock_gettime2(&ts);

	add_time_in_ms(&ts, time_in_ms);

	while ((err = sem_timedwait(&my_sem_stop_is_required, &ts)) == -1
	       && errno == EINTR)
		continue; // Restart when interrupted by handler

	assert(gettimeofday(&tv, NULL) != -1);

	if (err == 0)
		a_stop_is_required = 1; // stop required
	return a_stop_is_required;
}

// Asked for the time interval required for reaching the sample.
// If the stream is opened but the audio samples are not played,
// a timeout is started.

static int get_remaining_time(uint32_t sample, uint32_t *time_in_ms, int *stop_is_required)
{
	int err = 0;
	*stop_is_required = 0;
	int i = 0;

	for (i = 0; i < MAX_ACTIVITY_CHECK && (*stop_is_required == 0); i++) {
		err = wave_get_remaining_time(sample, time_in_ms);

		if (err ||                // if err, stream not available: quit
		    wave_is_busy(NULL) || // if wave is busy, time_in_ms is known: quit
		    (*time_in_ms == 0)) { // if wave is not busy but remaining time == 0, event is reached: quit
			break;
		}

		// stream opened but not active
		//
		// Several possible states:
		//   * the stream is opened but not yet started:
		//
		//       wait for the start of stream
		//
		//   * some samples have already been played,
		//      ** the end of stream is reached
		//      ** or there is an underrun
		//
		//       wait for the close of stream

		*stop_is_required = sleep_until_timeout_or_stop_request(ACTIVITY_TIMEOUT);
	}

	return err;
}

static void *polling_thread(void *p)
{
	(void)p; // unused

	while (1) {
		int a_stop_is_required = 0;

		int a_status = pthread_mutex_lock(&my_mutex);
		my_event_is_running = 0;
		pthread_mutex_unlock(&my_mutex);

		while ((sem_wait(&my_sem_start_is_required) == -1) && errno == EINTR)
			continue; // Restart when interrupted by handler

		a_status = pthread_mutex_lock(&my_mutex);
		my_event_is_running = 1;
		pthread_mutex_unlock(&my_mutex);

		a_stop_is_required = 0;
		a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required); // NOTE: may set a_stop_is_required to -1
		if ((a_status == 0) && (a_stop_is_required > 0)) {
			while (0 == sem_trywait(&my_sem_stop_is_required))
				;
		} else
			a_stop_is_required = 0;

		// In this loop, my_event_is_running = 1
		while (head && (a_stop_is_required <= 0)) {
			while (0 == sem_trywait(&my_sem_start_is_required))
				;

			espeak_EVENT *event = (espeak_EVENT *)(head->data);
			assert(event);

			uint32_t time_in_ms = 0;

			int err = get_remaining_time((uint32_t)event->sample,
			                             &time_in_ms,
			                             &a_stop_is_required);
			if (a_stop_is_required > 0)
				break;
			else if (err != 0) {
				// No available time: the event is deleted.
				a_status = pthread_mutex_lock(&my_mutex);
				event_delete((espeak_EVENT *)pop());
				a_status = pthread_mutex_unlock(&my_mutex);
			} else if (time_in_ms == 0) {   // the event is already reached.
				if (my_callback) {
					event_notify(event);
					// the user_data (and the type) are cleaned to be sure
					// that MSG_TERMINATED is called twice (at delete time too).
					event->type = espeakEVENT_LIST_TERMINATED;
					event->user_data = NULL;
				}

				a_status = pthread_mutex_lock(&my_mutex);
				event_delete((espeak_EVENT *)pop());
				a_status = pthread_mutex_unlock(&my_mutex);

				a_stop_is_required = 0;
				a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required);

				if ((a_status == 0) && (a_stop_is_required > 0)) {
					while (0 == sem_trywait(&my_sem_stop_is_required))
						;
				} else
					a_stop_is_required = 0;
			} else // The event will be notified soon: sleep until timeout or stop request
				a_stop_is_required = sleep_until_timeout_or_stop_request(time_in_ms);
		}

		a_status = pthread_mutex_lock(&my_mutex);

		my_event_is_running = 0;

		if (a_stop_is_required <= 0) {
			a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required);
			if ((a_status == 0) && (a_stop_is_required > 0)) {
				while (0 == sem_trywait(&my_sem_stop_is_required))
					;
			} else
				a_stop_is_required = 0;
		}

		a_status = pthread_mutex_unlock(&my_mutex);

		if (a_stop_is_required > 0) {
			// no mutex required since the stop command is synchronous
			// and waiting for my_sem_stop_is_acknowledged
			init();

			// acknowledge the stop request
			a_status = sem_post(&my_sem_stop_is_acknowledged);
		}
	}

	return NULL;
}

enum { MAX_NODE_COUNTER = 1000 };

static espeak_ng_STATUS push(void *the_data)
{
	assert((!head && !tail) || (head && tail));

	if (the_data == NULL)
		return EINVAL;

	if (node_counter >= MAX_NODE_COUNTER)
		return ENS_EVENT_BUFFER_FULL;

	node *n = (node *)malloc(sizeof(node));
	if (n == NULL)
		return ENOMEM;

	if (head == NULL) {
		head = n;
		tail = n;
	} else {
		tail->next = n;
		tail = n;
	}

	tail->next = NULL;
	tail->data = the_data;

	node_counter++;

	return ENS_OK;
}

static void *pop()
{
	void *the_data = NULL;

	assert((!head && !tail) || (head && tail));

	if (head != NULL) {
		node *n = head;
		the_data = n->data;
		head = n->next;
		free(n);
		node_counter--;
	}

	if (head == NULL)
		tail = NULL;

	return the_data;
}


static void init()
{
	while (event_delete((espeak_EVENT *)pop()))
		;

	node_counter = 0;
}

void event_terminate()
{
	if (thread_inited) {
		pthread_cancel(my_thread);
		pthread_join(my_thread, NULL);
		pthread_mutex_destroy(&my_mutex);
		sem_destroy(&my_sem_start_is_required);
		sem_destroy(&my_sem_stop_is_required);
		sem_destroy(&my_sem_stop_is_acknowledged);
		init(); // purge event
		thread_inited = 0;
	}
}

enum { ONE_BILLION = 1000000000 };

void clock_gettime2(struct timespec *ts)
{
	struct timeval tv;

	if (!ts)
		return;

	assert(gettimeofday(&tv, NULL) != -1);
	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec*1000;
}

void add_time_in_ms(struct timespec *ts, int time_in_ms)
{
	if (!ts)
		return;

	uint64_t t_ns = (uint64_t)ts->tv_nsec + 1000000 * (uint64_t)time_in_ms;
	while (t_ns >= ONE_BILLION) {
		ts->tv_sec += 1;
		t_ns -= ONE_BILLION;
	}
	ts->tv_nsec = (long int)t_ns;
}
