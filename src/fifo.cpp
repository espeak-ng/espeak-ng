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
#include <wchar.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "fifo.h"
#include "wave.h"
#include "debug.h"


//>
//<decls and function prototypes

// my_mutex: protects my_thread_is_talking, 
// my_stop_is_required, and the command fifo
static pthread_mutex_t my_mutex;
static int my_command_is_running = 0;
static int my_stop_is_required = 0;
// + fifo
//

// my_thread: reads commands from the fifo, and runs them. 
static pthread_t my_thread;
static sem_t my_sem_start_is_required;
static sem_t my_sem_stop_is_acknowledged;

static void* say_thread(void*);

static espeak_ERROR push(t_espeak_command* the_command);
static t_espeak_command* pop();
static void init();
static int node_counter=0;
enum {MAX_NODE_COUNTER=400,
      INACTIVITY_TIMEOUT=50, // in ms, check that the stream is inactive
      MAX_INACTIVITY_CHECK=2
};

//>
//<fifo_init
void fifo_init()
{
  ENTER("fifo_init");

  // security
  pthread_mutex_init( &my_mutex, (const pthread_mutexattr_t *)NULL);
  init();

  assert(-1 != sem_init(&my_sem_start_is_required, 0, 0));
  assert(-1 != sem_init(&my_sem_stop_is_acknowledged, 0, 0));

  pthread_attr_t a_attrib;    
  if (pthread_attr_init (& a_attrib)
      || pthread_attr_setdetachstate(&a_attrib, PTHREAD_CREATE_JOINABLE)
      || pthread_create( &my_thread, 
			 & a_attrib, 
			 say_thread, 
			 (void*)NULL))
    {
      assert(0);
    }

  pthread_attr_destroy(&a_attrib);

  // leave once the thread is actually started
  SHOW_TIME("fifo > wait for my_sem_stop_is_acknowledged\n");
  while ((sem_wait(&my_sem_stop_is_acknowledged) == -1) && errno == EINTR)
    {
      continue; // Restart when interrupted by handler
    }
  SHOW_TIME("fifo > get my_sem_stop_is_acknowledged\n");
}
//>
//<fifo_add_command

espeak_ERROR fifo_add_command (t_espeak_command* the_command)
{
  ENTER("fifo_add_command");
	  
  int a_status = pthread_mutex_lock(&my_mutex); 
  espeak_ERROR a_error = EE_OK;

  if (!a_status)
    {
      SHOW_TIME("fifo_add_command > locked\n");
      a_error = push(the_command);
      SHOW_TIME("fifo_add_command > unlocking\n");
      a_status = pthread_mutex_unlock(&my_mutex);
    }

  if (!a_status && !my_command_is_running && (a_error == EE_OK))
    {
      // quit when command is actually started 
      // (for possible forthcoming 'end of command' checks)
      SHOW_TIME("fifo_add_command > post my_sem_start_is_required\n");
      sem_post(&my_sem_start_is_required);
      int val=1;
      while (val)
	{
	  usleep(50000); // TBD: event?
	  sem_getvalue(&my_sem_start_is_required, &val);
	}
    }

  if (a_status != 0)
    {
      a_error = EE_INTERNAL_ERROR;
    }

  SHOW_TIME("LEAVE fifo_add_command");
  return a_error;
}

//>
//<fifo_add_commands

espeak_ERROR fifo_add_commands (t_espeak_command* command1, t_espeak_command* command2)
{
  ENTER("fifo_add_command");
	  
  int a_status = pthread_mutex_lock(&my_mutex); 
  espeak_ERROR a_error = EE_OK;

  if (!a_status)
    {
      SHOW_TIME("fifo_add_commands > locked\n");

      if (node_counter+1 >= MAX_NODE_COUNTER)
	{
	  SHOW("push > %s\n", "EE_BUFFER_FULL");
	  a_error = EE_BUFFER_FULL;
	}
      else
	{
	  push(command1);
	  push(command2);
	}
      SHOW_TIME("fifo_add_command > unlocking\n");
      a_status = pthread_mutex_unlock(&my_mutex);
    }

  if (!a_status && !my_command_is_running && (a_error == EE_OK))
    {
      // quit when one command is actually started 
      // (for possible forthcoming 'end of command' checks)
      SHOW_TIME("fifo_add_command > post my_sem_start_is_required\n");
      sem_post(&my_sem_start_is_required);
      int val=1;
      while (val)
	{
	  usleep(50000); // TBD: event?
	  sem_getvalue(&my_sem_start_is_required, &val);
	}
    }

  if (a_status != 0)
    {
      a_error = EE_INTERNAL_ERROR;
    }

  SHOW_TIME("LEAVE fifo_add_commands");
  return a_error;
}

//>
//<fifo_stop

espeak_ERROR fifo_stop ()
{
  ENTER("fifo_stop");

  int a_command_is_running = 0;
  int a_status = pthread_mutex_lock(&my_mutex);
  SHOW_TIME("fifo_stop > locked\n");
  if (a_status != 0)
    {
      return EE_INTERNAL_ERROR;
    }

  if (my_command_is_running)
    {
      a_command_is_running = 1;
      my_stop_is_required = 1;
      SHOW_TIME("fifo_stop > my_stop_is_required = 1\n");
    }
  SHOW_TIME("fifo_stop > unlocking\n");
  a_status = pthread_mutex_unlock(&my_mutex);
  if (a_status != 0)
    {
      return EE_INTERNAL_ERROR;
    }

  if (a_command_is_running)
    {
      SHOW_TIME("fifo_stop > wait for my_sem_stop_is_acknowledged\n");
      while ((sem_wait(&my_sem_stop_is_acknowledged) == -1) && errno == EINTR)
	{
	  continue; // Restart when interrupted by handler
	}
      SHOW_TIME("fifo_stop > get my_sem_stop_is_acknowledged\n");
    }

  SHOW_TIME("fifo_stop > my_stop_is_required = 0\n");
  my_stop_is_required = 0;
  SHOW_TIME("LEAVE fifo_stop\n");

  return EE_OK;
}

//>

//<fifo_is_speaking
int fifo_is_busy ()
{
  //  ENTER("isSpeaking");
  //  int aResult = (int) (my_command_is_running || WaveIsPlaying());
  SHOW("fifo_is_busy > aResult = %d\n",my_command_is_running);
  return my_command_is_running;
}

// int pause ()
// {
//   ENTER("pause");
//   // TBD
//   //   if (espeakPause (espeakHandle, 1))
//   return true;
// }

// int resume ()
// {
//   ENTER("resume");
//   // TBD
//   //   if (espeakPause (espeakHandle, 0))
//   return true;
// }
//>


//<sleep_until_start_request_or_inactivity

static int sleep_until_start_request_or_inactivity()
{
  SHOW_TIME("fifo > sleep_until_start_request_or_inactivity > ENTER");
  int a_start_is_required=0;

  // Wait for the start request (my_sem_start_is_required).
  // Besides this, if the audio stream is still busy, 
  // check from time to time its end. 
  // The end of the stream is confirmed by several checks 
  // for filtering underflow.
  //
  int i=0;
  while((i<= MAX_INACTIVITY_CHECK) && !a_start_is_required)
    {
      if (wave_is_busy( NULL) )
	{
	  i = 0;
	}
      else
	{
	  i++;
	}

      int err=0;
      struct timespec ts, to;
      struct timeval tv;
      
      clock_gettime2( &ts);
      to.tv_sec = ts.tv_sec;
      to.tv_nsec = ts.tv_nsec;
      
      add_time_in_ms( &ts, INACTIVITY_TIMEOUT);
      
      SHOW("fifo > sleep_until_start_request_or_inactivity > start sem_timedwait (start_is_required) from %d.%09lu to %d.%09lu \n", 
	   to.tv_sec, to.tv_nsec,
	   ts.tv_sec, ts.tv_nsec);
      
      while ((err = sem_timedwait(&my_sem_start_is_required, &ts)) == -1 
	     && errno == EINTR)
	{
	      continue;
	}
      
      assert (gettimeofday(&tv, NULL) != -1);
      SHOW("fifo > sleep_until_start_request_or_inactivity > stop sem_timedwait (start_is_required, err=%d) %d.%09lu \n", err, 
	   tv.tv_sec, tv.tv_usec*1000);
      
      if (err==0)
	{
	  a_start_is_required = 1;
	}
    }
  SHOW_TIME("fifo > sleep_until_start_request_or_inactivity > LEAVE");
  return a_start_is_required;
}

//>
//<close_stream

static void close_stream()
{
  SHOW_TIME("fifo > close_stream > ENTER\n");

  // Warning: a wave_close can be already required by 
  // an external command (espeak_Cancel + fifo_stop), if so: 
  // my_stop_is_required = 1;

  int a_status = pthread_mutex_lock(&my_mutex);
  assert (!a_status);
  int a_stop_is_required = my_stop_is_required;
  if (!a_stop_is_required)
    {
      my_command_is_running = 1;
    }
  a_status = pthread_mutex_unlock(&my_mutex);

  if (!a_stop_is_required)
    {
      wave_close(NULL);

      int a_status = pthread_mutex_lock(&my_mutex);
      assert (!a_status);
      my_command_is_running = 0;

      a_stop_is_required = my_stop_is_required;
      a_status = pthread_mutex_unlock(&my_mutex);
      
      if (a_stop_is_required)
	{
	  // acknowledge the stop request
	  SHOW_TIME("fifo > close_stream > post my_sem_stop_is_acknowledged\n");
	  int a_status = sem_post(&my_sem_stop_is_acknowledged);
	  assert( a_status != -1);
	}
    }

  SHOW_TIME("fifo > close_stream > LEAVE\n");
}

//>
//<say_thread

static void* say_thread(void*)
{
  ENTER("say_thread");

  SHOW_TIME("say_thread > post my_sem_stop_is_acknowledged\n");

  // announce that thread is started
  sem_post(&my_sem_stop_is_acknowledged);

  int look_for_inactivity=0;

  while(1)
    {
      SHOW_TIME("say_thread > wait for my_sem_start_is_required\n");

      int a_start_is_required = 0;
      if (look_for_inactivity)
	{
	  a_start_is_required = sleep_until_start_request_or_inactivity();
	  if (!a_start_is_required)
	    {
	      close_stream();
	    }
	}
      look_for_inactivity = 1;

      if (!a_start_is_required)
	{
	  while ((sem_wait(&my_sem_start_is_required) == -1) && errno == EINTR)
	    {
	      continue; // Restart when interrupted by handler
	    }
	}
      SHOW_TIME("say_thread > get my_sem_start_is_required\n");

      SHOW_TIME("say_thread > my_command_is_running = 1\n");
      my_command_is_running = 1;

      while( my_command_is_running)
	{
	  SHOW_TIME("say_thread > locking\n");
	  int a_status = pthread_mutex_lock(&my_mutex);
	  assert (!a_status);
	  t_espeak_command* a_command = (t_espeak_command*)pop();

	  if (a_command == NULL)
	    {
	      SHOW_TIME("say_thread > text empty (talking=0) \n");
	      a_status = pthread_mutex_unlock(&my_mutex);
	      SHOW_TIME("say_thread > unlocked\n");
	      SHOW_TIME("say_thread > my_command_is_running = 0\n");
	      my_command_is_running = 0;
	    }
	  else
	    {
	      display_espeak_command(a_command);
	      // purge start semaphore
	      SHOW_TIME("say_thread > purge my_sem_start_is_required\n");
	      while(0 == sem_trywait(&my_sem_start_is_required))
		{
		};

	      if (my_stop_is_required)
		{ 
		  SHOW_TIME("say_thread > my_command_is_running = 0\n");
		  my_command_is_running = 0;
		}
	      SHOW_TIME("say_thread > unlocking\n");
	      a_status = pthread_mutex_unlock(&my_mutex);

	      if (my_command_is_running)
		{
		  process_espeak_command(a_command);
		}
	      delete_espeak_command(a_command);
	    }
	}

      if (my_stop_is_required)
	{ 
	  // no mutex required since the stop command is synchronous
	  // and waiting for my_sem_stop_is_acknowledged
	  init();

	  // purge start semaphore
	  SHOW_TIME("say_thread > purge my_sem_start_is_required\n");
	  while(0==sem_trywait(&my_sem_start_is_required))
	    {
	    };
	  
	  // acknowledge the stop request
	  SHOW_TIME("say_thread > post my_sem_stop_is_acknowledged\n");
	  int a_status = sem_post(&my_sem_stop_is_acknowledged);
	  assert( a_status != -1);
	}
      // and wait for the next start
      SHOW_TIME("say_thread > wait for my_sem_start_is_required\n");      
    }
    
  return NULL;
}

int fifo_is_command_enabled()
{
  SHOW("ENTER fifo_is_command_enabled=%d\n",(int)(0 == my_stop_is_required));
  return (0 == my_stop_is_required);
}

//>
//<fifo
typedef struct t_node
{
  t_espeak_command* data;
  t_node *next;
} node;

static node* head=NULL;
static node* tail=NULL;
// return 1 if ok, 0 otherwise
static espeak_ERROR push(t_espeak_command* the_command)
{
  ENTER("fifo > push");

  assert((!head && !tail) || (head && tail));

  if (the_command == NULL)
    {
      SHOW("push > command=0x%x\n", NULL);
      return EE_INTERNAL_ERROR;
    }

  if (node_counter >= MAX_NODE_COUNTER)
    {
      SHOW("push > %s\n", "EE_BUFFER_FULL");
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
  tail->data = the_command;

  node_counter++;
  SHOW("push > counter=%d\n",node_counter);

  the_command->state = CS_PENDING;
  display_espeak_command(the_command);

  return EE_OK;
}

static t_espeak_command* pop()
{
  ENTER("fifo > pop");
  t_espeak_command* the_command = NULL;

  assert((!head && !tail) || (head && tail));

  if (head != NULL)
    {
      node* n = head;
      the_command = n->data;
      head = n->next;
      free(n);
      node_counter--;
      SHOW("pop > command=0x%x (counter=%d)\n",the_command, node_counter);
    }

  if(head == NULL)
    {
      tail = NULL;
    }

  display_espeak_command(the_command);
 
  return the_command;
}


static void init()
{
  ENTER("fifo > init");
  while (delete_espeak_command( pop() ))
    {}
  node_counter = 0;
}

//>
//<fifo_init
void fifo_terminate()
{
  ENTER("fifo_terminate");

  pthread_cancel(my_thread);
  pthread_join(my_thread,NULL);
  pthread_mutex_destroy(&my_mutex);
  sem_destroy(&my_sem_start_is_required);
  sem_destroy(&my_sem_stop_is_acknowledged);

  init(); // purge fifo
}

#endif
//>

