/***************************************************************************
 *   Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>                  *
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


//<includes
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>

#include "speak_lib.h"
#include "event.h"
#include "wave.h"
#include "debug.h"
//>
//<decls and function prototypes


// my_mutex: protects my_thread_is_talking, 
static pthread_mutex_t my_mutex;
static sem_t my_sem_start_is_required;
static sem_t my_sem_stop_is_required;
static sem_t my_sem_stop_is_acknowledged;
// my_thread: polls the audio duration and compares it to the duration of the first event.
static pthread_t my_thread;

static t_espeak_callback* my_callback = NULL;
static int my_event_is_running=0;

enum {MIN_TIMEOUT_IN_MS=10,
      ACTIVITY_TIMEOUT=50, // in ms, check that the stream is active
      MAX_ACTIVITY_CHECK=6 
};


typedef struct t_node
{
  void* data;
  t_node *next;
} node;

static node* head=NULL;
static node* tail=NULL;
static int node_counter=0;
static espeak_ERROR push(void* data);
static void* pop();
static void init();
static void* polling_thread(void*);

//>
//<event_init

void event_set_callback(t_espeak_callback* SynthCallback)
{
  my_callback = SynthCallback;
}

void event_init(void)
{
  ENTER("event_init");

  my_event_is_running=0;

  // security
  pthread_mutex_init( &my_mutex, (const pthread_mutexattr_t *)NULL);
  init();

  assert(-1 != sem_init(&my_sem_start_is_required, 0, 0));
  assert(-1 != sem_init(&my_sem_stop_is_required, 0, 0));
  assert(-1 != sem_init(&my_sem_stop_is_acknowledged, 0, 0));

  pthread_attr_t a_attrib;    
  if (pthread_attr_init (& a_attrib)
      || pthread_attr_setdetachstate(&a_attrib, PTHREAD_CREATE_JOINABLE)
      || pthread_create( &my_thread, 
			 & a_attrib, 
			 polling_thread, 
			 (void*)NULL))
    {
      assert(0);
    }

  pthread_attr_destroy(&a_attrib);
}
//>
//<event_display
static void event_display(espeak_EVENT* event)
{
ENTER("event_display");

#ifdef DEBUG_ENABLED
	if (event==NULL)
	{
		SHOW("event_display > event=%s\n","NULL");
	}
	else
	{
		static const char* label[] = {
		"LIST_TERMINATED",
		"WORD",
		"SENTENCE",
		"MARK",
		"PLAY",
		"END",
		"MSG_TERMINATED"  
		};

		SHOW("event_display > event=0x%x\n",event);
		SHOW("event_display >   type=%s\n",label[event->type]);
		SHOW("event_display >   uid=%d\n",event->unique_identifier);
		SHOW("event_display >   text_position=%d\n",event->text_position);
		SHOW("event_display >   length=%d\n",event->length);
		SHOW("event_display >   audio_position=%d\n",event->audio_position);
		SHOW("event_display >   sample=%d\n",event->sample);
		SHOW("event_display >   user_data=0x%x\n",event->user_data);
	}
#endif
}
//>
//<event_copy

static espeak_EVENT* event_copy (espeak_EVENT* event)
{
	ENTER("event_copy");
	
	if (event==NULL)
	{
		return NULL;
	}
	
	espeak_EVENT* a_event=(espeak_EVENT*)malloc(sizeof(espeak_EVENT));
	if (a_event)
	{
		memcpy(a_event, event, sizeof(espeak_EVENT));
	
		switch(event->type)
		{
		case espeakEVENT_MARK:
		case espeakEVENT_PLAY:
			if (event->id.name)
			{
				a_event->id.name = strdup(event->id.name);
			}
		break;
		
		default:
			break;
		}
	}
	
	event_display(a_event);
	
	return a_event;
}

//>
//<event_notify

// Call the user supplied callback
//
// Note: the current sequence is:
//
// * First call with: event->type = espeakEVENT_SENTENCE
// * 0, 1 or several calls: event->type = espeakEVENT_WORD
// * Last call: event->type = espeakEVENT_MSG_TERMINATED
//

static void event_notify(espeak_EVENT* event)
{
ENTER("event_notify");
	static unsigned int a_old_uid = 0;

	espeak_EVENT events[2];
	memcpy(&events[0],event,sizeof(espeak_EVENT));     // the event parameter in the callback function should be an array of eventd
	memcpy(&events[1],event,sizeof(espeak_EVENT));
	events[1].type = espeakEVENT_LIST_TERMINATED;           // ... terminated by an event type=0

	if (event && my_callback)
	{
		event_display(event);

		switch(event->type)
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
// jonsd - I'm not sure what this is for. gilles says it's for when Gnome Speech reads a file of blank lines
			if (a_old_uid != event->unique_identifier)
			{
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

		default:
			case espeakEVENT_LIST_TERMINATED:
			case espeakEVENT_PLAY:
		break;
		}
	}
}
//>
//<event_delete

static int event_delete(espeak_EVENT* event)
{
ENTER("event_delete");

	event_display(event);

	if(event==NULL)
	{
		return 0;
	}

	switch(event->type)
	{
	case espeakEVENT_MSG_TERMINATED:
		event_notify(event);
		break;

	case espeakEVENT_MARK:
	case espeakEVENT_PLAY:
		if(event->id.name)
		{
			free((void*)(event->id.name));
		}
		break;

	default:
		break;
	}

	free(event);
	return 1;
}

//>
//<event_declare

espeak_ERROR event_declare (espeak_EVENT* event)
{
ENTER("event_declare");

	event_display(event);

	if (!event)
	{
		return EE_INTERNAL_ERROR;
	}

	int a_status = pthread_mutex_lock(&my_mutex); 
	espeak_ERROR a_error = EE_OK;

	if (!a_status)
	{
		SHOW_TIME("event_declare > locked\n");
		espeak_EVENT* a_event = event_copy(event);
		a_error = push(a_event);
		if (a_error != EE_OK)
		{
			event_delete(a_event);
		}
		SHOW_TIME("event_declare > unlocking\n");
		a_status = pthread_mutex_unlock(&my_mutex);
	}

  // TBD: remove the comment
  // reminder: code in comment.
  // This wait can lead to an underrun
  //
//   if (!a_status && !my_event_is_running && (a_error == EE_OK))
//   {
//       // quit when command is actually started 
//       // (for possible forthcoming 'end of command' checks)
	SHOW_TIME("event_declare > post my_sem_start_is_required\n");
	sem_post(&my_sem_start_is_required);
//       int val=1;
//       while (val)
// 	{
// 	  usleep(50000); // TBD: event?
// 	  sem_getvalue(&my_sem_start_is_required, &val);
// 	}
//     }

	if (a_status != 0)
	{
		a_error = EE_INTERNAL_ERROR;
	}

	return a_error;
}

//>
//<event_clear_all

espeak_ERROR event_clear_all ()
{
	ENTER("event_clear_all");

	int a_status = pthread_mutex_lock(&my_mutex);
	int a_event_is_running = 0;

	SHOW_TIME("event_stop > locked\n");
	if (a_status != 0)
	{
		return EE_INTERNAL_ERROR;
	}

	if (my_event_is_running)
	{
		SHOW_TIME("event_stop > post my_sem_stop_is_required\n");
		sem_post(&my_sem_stop_is_required);
		a_event_is_running = 1;
	}
	else
	{
		init(); // clear pending events
	}
	SHOW_TIME("event_stop > unlocking\n");
	a_status = pthread_mutex_unlock(&my_mutex);
	if (a_status != 0)
	{
		return EE_INTERNAL_ERROR;
	}

	if (a_event_is_running)
	{
		SHOW_TIME("event_stop > wait for my_sem_stop_is_acknowledged\n");
		while ((sem_wait(&my_sem_stop_is_acknowledged) == -1) && errno == EINTR)
		{
			continue; // Restart when interrupted by handler
		}
		SHOW_TIME("event_stop > get my_sem_stop_is_acknowledged\n");
	}

	SHOW_TIME("LEAVE event_stop\n");

	return EE_OK;
}

//>
//<sleep_until_timeout_or_stop_request

static int sleep_until_timeout_or_stop_request(uint32_t time_in_ms)
{
ENTER("sleep_until_timeout_or_stop_request");

	int a_stop_is_required=0;
	struct timespec ts, to;
	struct timeval tv;
	int err=0;

	clock_gettime2( &ts);
	to.tv_sec = ts.tv_sec;
	to.tv_nsec = ts.tv_nsec;

	add_time_in_ms( &ts, time_in_ms);

	SHOW("polling_thread > sleep_until_timeout_or_stop_request > start sem_timedwait from %d.%09lu to %d.%09lu \n", 
       to.tv_sec, to.tv_nsec,
       ts.tv_sec, ts.tv_nsec);

	while ((err = sem_timedwait(&my_sem_stop_is_required, &ts)) == -1 
		&& errno == EINTR)
	{
		continue; // Restart when interrupted by handler
	}

	assert (gettimeofday(&tv, NULL) != -1);
	SHOW("polling_thread > sleep_until_timeout_or_stop_request > stop sem_timedwait %d.%09lu \n", 
       tv.tv_sec, tv.tv_usec*1000);

	if (err == 0)
	{
		SHOW("polling_thread > sleep_until_timeout_or_stop_request > %s\n","stop required!");
		a_stop_is_required=1; // stop required
	}
	return a_stop_is_required;
}

//>
//<get_remaining_time
// Asked for the time interval required for reaching the sample.
// If the stream is opened but the audio samples are not played, 
// a timeout is started.

static int get_remaining_time(uint32_t sample, uint32_t* time_in_ms, int* stop_is_required)
{
ENTER("get_remaining_time");

	int err = 0;
	*stop_is_required = 0;
	int i=0;

	for (i=0; i < MAX_ACTIVITY_CHECK && (*stop_is_required == 0); i++)
	{
		err = wave_get_remaining_time( sample, time_in_ms);

		if (err || wave_is_busy(NULL) || (*time_in_ms == 0))
		{ // if err, stream not available: quit
	  // if wave is busy, time_in_ms is known: quit
	  // if wave is not busy but remaining time == 0, event is reached: quit
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

		*stop_is_required = sleep_until_timeout_or_stop_request( ACTIVITY_TIMEOUT);      
	}

	return err;
}

//>
//<polling_thread

static void* polling_thread(void*)
{
ENTER("polling_thread");

	while(1)
	{
		int a_stop_is_required=0;

		SHOW_TIME("polling_thread > locking\n");
		int a_status = pthread_mutex_lock(&my_mutex);
		SHOW_TIME("polling_thread > locked (my_event_is_running = 0)\n");
		my_event_is_running = 0;
		pthread_mutex_unlock(&my_mutex);
		SHOW_TIME("polling_thread > unlocked\n");

		SHOW_TIME("polling_thread > wait for my_sem_start_is_required\n");

		while ((sem_wait(&my_sem_start_is_required) == -1) && errno == EINTR)
		{
			continue; // Restart when interrupted by handler
		}

		SHOW_TIME("polling_thread > get my_sem_start_is_required\n");

		a_status = pthread_mutex_lock(&my_mutex);
		SHOW_TIME("polling_thread > locked (my_event_is_running = 1)\n");
		my_event_is_running = 1;
		pthread_mutex_unlock(&my_mutex);
		SHOW_TIME("polling_thread > unlocked\n");

		a_stop_is_required=0;
		a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required);
		if ((a_status==0) && a_stop_is_required)
		{
			SHOW("polling_thread > stop required (%d)\n", __LINE__);
			while(0 == sem_trywait(&my_sem_stop_is_required))
			{
			};
		}
		else
		{
			a_stop_is_required=0;
		}

		// In this loop, my_event_is_running = 1
		while (head && !a_stop_is_required)
		{
			SHOW_TIME("polling_thread > check head\n");
			while(0 == sem_trywait(&my_sem_start_is_required))
			{
			};
	
			espeak_EVENT* event = (espeak_EVENT*)(head->data);
			assert(event);
	
			uint32_t time_in_ms = 0;
	
			int err = get_remaining_time((uint32_t)event->sample, 
							&time_in_ms, 
							&a_stop_is_required);
			if (a_stop_is_required)
			{
				break;
			}
			else if (err != 0)
			{ 
				// No available time: the event is deleted.
				SHOW("polling_thread > %s\n","audio device down");
				a_status = pthread_mutex_lock(&my_mutex);
				SHOW_TIME("polling_thread > locked\n");
				event_delete( (espeak_EVENT*)pop());
				a_status = pthread_mutex_unlock(&my_mutex);
				SHOW_TIME("polling_thread > unlocked\n");
			}
			else if (time_in_ms==0)
			{ // the event is already reached.
				if (my_callback)
				{
					event_notify(event);
					// the user_data (and the type) are cleaned to be sure 
					// that MSG_TERMINATED is called twice (at delete time too).
					event->type=espeakEVENT_LIST_TERMINATED;
					event->user_data=NULL;
				}
	
				a_status = pthread_mutex_lock(&my_mutex);
				SHOW_TIME("polling_thread > locked\n");
				event_delete( (espeak_EVENT*)pop());
				a_status = pthread_mutex_unlock(&my_mutex);
				SHOW_TIME("polling_thread > unlocked\n");
			
				a_stop_is_required=0;
				a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required);
	
				if ((a_status==0) && a_stop_is_required)
				{
					SHOW("polling_thread > stop required (%d)\n", __LINE__);
					while(0 == sem_trywait(&my_sem_stop_is_required))
					{
					};
				}
				else
				{
					a_stop_is_required=0;
				}
			}
			else
			{ // The event will be notified soon: sleep until timeout or stop request
				a_stop_is_required = sleep_until_timeout_or_stop_request(time_in_ms);
			}
		}
	
		a_status = pthread_mutex_lock(&my_mutex);
		SHOW_TIME("polling_thread > locked\n");
	
		SHOW_TIME("polling_thread > my_event_is_running = 0\n");
		my_event_is_running = 0;
	
		if(!a_stop_is_required)
		{
			a_status = sem_getvalue(&my_sem_stop_is_required, &a_stop_is_required);
			if ((a_status==0) && a_stop_is_required)
			{
				SHOW("polling_thread > stop required (%d)\n", __LINE__);
				while(0 == sem_trywait(&my_sem_stop_is_required))
				{
				};
			}
			else
			{
				a_stop_is_required=0;
			}
		}

		a_status = pthread_mutex_unlock(&my_mutex);
		SHOW_TIME("polling_thread > unlocked\n");

		if (a_stop_is_required)
		{ 
			SHOW("polling_thread > %s\n","stop required!");
			// no mutex required since the stop command is synchronous
			// and waiting for my_sem_stop_is_acknowledged
			init();

			// acknowledge the stop request
			SHOW_TIME("polling_thread > post my_sem_stop_is_acknowledged\n");
			a_status = sem_post(&my_sem_stop_is_acknowledged);
		}
	}

	return NULL;
}

//>
//<push, pop, init
enum {MAX_NODE_COUNTER=1000};
// return 1 if ok, 0 otherwise
static espeak_ERROR push(void* the_data)
{
	ENTER("event > push");

	assert((!head && !tail) || (head && tail));

	if (the_data == NULL)
	{
		SHOW("event > push > event=0x%x\n", NULL);
		return EE_INTERNAL_ERROR;
	}

	if (node_counter >= MAX_NODE_COUNTER)
	{
		SHOW("event > push > %s\n", "EE_BUFFER_FULL");
		return EE_BUFFER_FULL;
	}
	
	node *n = (node *)malloc(sizeof(node));
	if (n == NULL)
	{
		return EE_INTERNAL_ERROR;
	}
	
	if (head == NULL)
	{
		head = n;
		tail = n;
	}
	else
	{
		tail->next = n;
		tail = n;
	}
	
	tail->next = NULL;
	tail->data = the_data;
	
	node_counter++;
	SHOW("event > push > counter=%d (uid=%d)\n",node_counter,((espeak_EVENT*)the_data)->unique_identifier);
	
	return EE_OK;
}

static void* pop()
{
	ENTER("event > pop");
	void* the_data = NULL;
	
	assert((!head && !tail) || (head && tail));
	
	if (head != NULL)
	{
		node* n = head;
		the_data = n->data;
		head = n->next;
		free(n);
		node_counter--;
		SHOW("event > pop > event=0x%x (counter=%d, uid=%d)\n",the_data, node_counter,((espeak_EVENT*)the_data)->unique_identifier);
	}
	
	if(head == NULL)
	{
		tail = NULL;
	}
	
	return the_data;
}


static void init()
{
	ENTER("event > init");
	
	while (event_delete( (espeak_EVENT*)pop() ))
	{}
	
	node_counter = 0;
}

//>
//<event_terminate
void event_terminate()
{
ENTER("event_terminate");
	
	if (my_thread)
	{
		pthread_cancel(my_thread);
		pthread_join(my_thread,NULL);
		pthread_mutex_destroy(&my_mutex);
		sem_destroy(&my_sem_start_is_required);
		sem_destroy(&my_sem_stop_is_required);
		sem_destroy(&my_sem_stop_is_acknowledged);
		init(); // purge event
	}
}

#endif
//>

