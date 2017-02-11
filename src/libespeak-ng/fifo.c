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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <espeak-ng/espeak_ng.h>

#include "speech.h"
#include "espeak_command.h"
#include "fifo.h"
#include "event.h"

// my_mutex: protects my_thread_is_talking,
// my_stop_is_required, and the command fifo
static pthread_mutex_t my_mutex;
static int my_command_is_running = 0;
static pthread_cond_t my_cond_command_is_running;
static int my_stop_is_required = 0;

// my_thread: reads commands from the fifo, and runs them.
static pthread_t my_thread;

static pthread_cond_t my_cond_start_is_required;
static int my_start_is_required = 0;

static pthread_cond_t my_cond_stop_is_acknowledged;
static int my_stop_is_acknowledged = 0;

static void *say_thread(void *);

static espeak_ng_STATUS push(t_espeak_command *the_command);
static t_espeak_command *pop();
static void init(int process_parameters);
static int node_counter = 0;

enum {
	MAX_NODE_COUNTER = 400,
	INACTIVITY_TIMEOUT = 50, // in ms, check that the stream is inactive
	MAX_INACTIVITY_CHECK = 2
};

void fifo_init()
{
	// security
	pthread_mutex_init(&my_mutex, (const pthread_mutexattr_t *)NULL);
	init(0);

	assert(-1 != pthread_cond_init(&my_cond_command_is_running, NULL));
	assert(-1 != pthread_cond_init(&my_cond_start_is_required, NULL));
	assert(-1 != pthread_cond_init(&my_cond_stop_is_acknowledged, NULL));

	pthread_attr_t a_attrib;
	if (pthread_attr_init(&a_attrib)
	    || pthread_attr_setdetachstate(&a_attrib, PTHREAD_CREATE_JOINABLE)
	    || pthread_create(&my_thread,
	                      &a_attrib,
	                      say_thread,
	                      (void *)NULL)) {
		assert(0);
	}

	pthread_attr_destroy(&a_attrib);

	// leave once the thread is actually started
	assert(-1 != pthread_mutex_lock(&my_mutex));
	while (my_stop_is_acknowledged == 0) {
		while ((pthread_cond_wait(&my_cond_stop_is_acknowledged, &my_mutex) == -1) && errno == EINTR)
			;
	}
	my_stop_is_acknowledged = 0;
	pthread_mutex_unlock(&my_mutex);
}

espeak_ng_STATUS fifo_add_command(t_espeak_command *the_command)
{
	espeak_ng_STATUS status;
	if ((status = pthread_mutex_lock(&my_mutex)) != ENS_OK)
		return status;

	if ((status = push(the_command)) != ENS_OK) {
		pthread_mutex_unlock(&my_mutex);
		return status;
	}
	
	my_start_is_required = 1;
	pthread_cond_signal(&my_cond_start_is_required);

	while (my_start_is_required && !my_command_is_running) {
		if((status = pthread_cond_wait(&my_cond_command_is_running, &my_mutex)) != ENS_OK && errno != EINTR) {
			pthread_mutex_unlock(&my_mutex);
			return status;
		}
	}
	if ((status = pthread_mutex_unlock(&my_mutex)) != ENS_OK)
		return status;

	return ENS_OK;
}

espeak_ng_STATUS fifo_add_commands(t_espeak_command *command1, t_espeak_command *command2)
{
	espeak_ng_STATUS status;
	if ((status = pthread_mutex_lock(&my_mutex)) != ENS_OK)
		return status;

	if (node_counter+1 >= MAX_NODE_COUNTER) {
		pthread_mutex_unlock(&my_mutex);
		return ENS_FIFO_BUFFER_FULL;
	}

	if ((status = push(command1)) != ENS_OK) {
		pthread_mutex_unlock(&my_mutex);
		return status;
	}

	if ((status = push(command2)) != ENS_OK) {
		pthread_mutex_unlock(&my_mutex);
		return status;
	}

	my_start_is_required = 1;
	pthread_cond_signal(&my_cond_start_is_required);
	
	while (my_start_is_required && !my_command_is_running) {
		if((status = pthread_cond_wait(&my_cond_command_is_running, &my_mutex)) != ENS_OK && errno != EINTR) {
			pthread_mutex_unlock(&my_mutex);
			return status;
		}
	}
	if ((status = pthread_mutex_unlock(&my_mutex)) != ENS_OK)
		return status;

	return ENS_OK;
}

espeak_ng_STATUS fifo_stop()
{
	espeak_ng_STATUS status;
	if ((status = pthread_mutex_lock(&my_mutex)) != ENS_OK)
		return status;

	int a_command_is_running = 0;
	if (my_command_is_running) {
		a_command_is_running = 1;
		my_stop_is_required = 1;
		my_stop_is_acknowledged = 0;
	}

	if (a_command_is_running) {
		while (my_stop_is_acknowledged == 0) {
			while ((pthread_cond_wait(&my_cond_stop_is_acknowledged, &my_mutex) == -1) && errno == EINTR)
				continue; // Restart when interrupted by handler
		}
	}

	my_stop_is_required = 0;
	if ((status = pthread_mutex_unlock(&my_mutex)) != ENS_OK)
		return status;

	return ENS_OK;
}

int fifo_is_busy()
{
	return my_command_is_running;
}

static int sleep_until_start_request_or_inactivity()
{
	int a_start_is_required = 0;

	// Wait for the start request (my_cond_start_is_required).
	// Besides this, if the audio stream is still busy,
	// check from time to time its end.
	// The end of the stream is confirmed by several checks
	// for filtering underflow.
	//
	int i = 0;
	int err = pthread_mutex_lock(&my_mutex);
	assert(err != -1);
	while ((i <= MAX_INACTIVITY_CHECK) && !a_start_is_required) {
		i++;

		struct timespec ts;
		struct timeval tv;

		clock_gettime2(&ts);

		add_time_in_ms(&ts, INACTIVITY_TIMEOUT);

		while ((err = pthread_cond_timedwait(&my_cond_start_is_required, &my_mutex, &ts)) == -1
		       && errno == EINTR)
			continue;

		assert(gettimeofday(&tv, NULL) != -1);

		if (err == 0)
			a_start_is_required = 1;
	}
	pthread_mutex_unlock(&my_mutex);
	return a_start_is_required;
}

static espeak_ng_STATUS close_stream()
{
	espeak_ng_STATUS status = pthread_mutex_lock(&my_mutex);
	if (status != ENS_OK)
		return status;

	int a_stop_is_required = my_stop_is_required;
	if (!a_stop_is_required)
		my_command_is_running = 1;

	status = pthread_mutex_unlock(&my_mutex);

	if (!a_stop_is_required) {
		int a_status = pthread_mutex_lock(&my_mutex);
		if (status == ENS_OK)
			status = a_status;

		my_command_is_running = 0;
		a_stop_is_required = my_stop_is_required;

		a_status = pthread_mutex_unlock(&my_mutex);
		if (status == ENS_OK)
			status = a_status;

		if (a_stop_is_required) {
			// cancel the audio early, to be more responsive when using eSpeak NG
			// for audio.
			cancel_audio();

			// acknowledge the stop request
			if((a_status = pthread_mutex_lock(&my_mutex)) != ENS_OK)
				return a_status;

			my_stop_is_acknowledged = 1;
			a_status = pthread_cond_signal(&my_cond_stop_is_acknowledged);
			if(a_status != ENS_OK)
				return a_status;
			a_status = pthread_mutex_unlock(&my_mutex);
			if (status == ENS_OK)
				status = a_status;
			
		}
	}

	return status;
}

static void *say_thread(void *p)
{
	(void)p; // unused

	// announce that thread is started
	assert(-1 != pthread_mutex_lock(&my_mutex));
	my_stop_is_acknowledged = 1;
	assert(-1 != pthread_cond_signal(&my_cond_stop_is_acknowledged));
	assert(-1 != pthread_mutex_unlock(&my_mutex));

	int look_for_inactivity = 0;

	while (1) {
		int a_start_is_required = 0;
		if (look_for_inactivity) {
			a_start_is_required = sleep_until_start_request_or_inactivity();
			if (!a_start_is_required)
				close_stream();
		}
		look_for_inactivity = 1;

		int a_status = pthread_mutex_lock(&my_mutex);
		assert(!a_status);

		if (!a_start_is_required) {
			while (my_start_is_required == 0) {
				while ((pthread_cond_wait(&my_cond_start_is_required, &my_mutex) == -1) && errno == EINTR)
					continue; // Restart when interrupted by handler
			}
		}


		my_command_is_running = 1;

		assert(-1 != pthread_cond_broadcast(&my_cond_command_is_running));
		assert(-1 != pthread_mutex_unlock(&my_mutex));

		while (my_command_is_running) {
			int a_status = pthread_mutex_lock(&my_mutex);
			assert(!a_status);
			t_espeak_command *a_command = (t_espeak_command *)pop();

			if (a_command == NULL) {
				my_command_is_running = 0;
				a_status = pthread_mutex_unlock(&my_mutex);
			} else {
				my_start_is_required = 0;

				if (my_stop_is_required)
					my_command_is_running = 0;
				a_status = pthread_mutex_unlock(&my_mutex);

				if (my_command_is_running)
					process_espeak_command(a_command);
				delete_espeak_command(a_command);
			}
		}

		if (my_stop_is_required) {
			// no mutex required since the stop command is synchronous
			// and waiting for my_cond_stop_is_acknowledged
			init(1);

			assert(-1 != pthread_mutex_lock(&my_mutex));
			my_start_is_required = 0;

			// acknowledge the stop request
			my_stop_is_acknowledged = 1;
			int a_status = pthread_cond_signal(&my_cond_stop_is_acknowledged);
			assert(a_status != -1);
			pthread_mutex_unlock(&my_mutex);

		}
		// and wait for the next start
	}

	return NULL;
}

int fifo_is_command_enabled()
{
	return 0 == my_stop_is_required;
}

typedef struct t_node {
	t_espeak_command *data;
	struct t_node *next;
} node;

static node *head = NULL;
static node *tail = NULL;

static espeak_ng_STATUS push(t_espeak_command *the_command)
{
	assert((!head && !tail) || (head && tail));

	if (the_command == NULL)
		return EINVAL;

	if (node_counter >= MAX_NODE_COUNTER)
		return ENS_FIFO_BUFFER_FULL;

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
	tail->data = the_command;

	node_counter++;

	the_command->state = CS_PENDING;

	return ENS_OK;
}

static t_espeak_command *pop()
{
	t_espeak_command *the_command = NULL;

	assert((!head && !tail) || (head && tail));

	if (head != NULL) {
		node *n = head;
		the_command = n->data;
		head = n->next;
		free(n);
		node_counter--;
	}

	if (head == NULL)
		tail = NULL;

	return the_command;
}

static void init(int process_parameters)
{
	t_espeak_command *c = NULL;
	c = pop();
	while (c != NULL) {
		if (process_parameters && (c->type == ET_PARAMETER || c->type == ET_VOICE_NAME || c->type == ET_VOICE_SPEC))
			process_espeak_command(c);
		delete_espeak_command(c);
		c = pop();
	}
	node_counter = 0;
}

void fifo_terminate()
{
	pthread_cancel(my_thread);
	pthread_join(my_thread, NULL);
	pthread_mutex_destroy(&my_mutex);
	pthread_cond_destroy(&my_cond_start_is_required);
	pthread_cond_destroy(&my_cond_stop_is_acknowledged);

	init(0); // purge fifo
}
