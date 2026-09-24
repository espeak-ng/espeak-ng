#if defined(__APPLE__)

#include <stdlib.h>
#include <stdbool.h>
#include <sys/syslimits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <time.h>
#include <libproc.h>

#include "platform.h"

struct process_state {
	struct timespec last_check;
	struct proc_bsdinfo bsd_info;
};

int process_start_monitor(struct process *process)
{
	struct process_state *state = calloc(1, sizeof(struct process_state));
	if (!state)
		return -1;

	process->state = state;
	return 0;
}

void process_stop_monitor(struct process *process)
{
	if (!process || !process->state)
		return;

	free(process->state);
	process->state = NULL;

	return;
}

#define	timespecsub(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {				\
			(vsp)->tv_sec--;				\
			(vsp)->tv_nsec += 1000000000L;			\
		}							\
	} while (0)

#define NANOSECONDS_PER_MILLISECOND 1000000LL
#define MS_TO_NS(n) ((n) * NANOSECONDS_PER_MILLISECOND)

bool process_is_idle(struct process *process)
{
	(void) process;
	// XXX: need an operationally cheaper way to query if the process is idle
	// FIXME: blindly returning true for now to avoid dragging for 4s on startup
	return true;
#if 0
	if (!process || process->pid <= 0)
		return true;

	struct process_state *state = process->state;
	if (!state)
		return true;

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	struct timespec delta;
	timespecsub(&now, &state->last_check, &delta);

	if (delta.tv_sec == 0 && delta.tv_nsec < MS_TO_NS(30))
		goto skip;

	if (proc_pidinfo(process->pid, PROC_PIDTBSDINFO, 0, &state->bsd_info, sizeof(state->bsd_info)) != sizeof(state->bsd_info)) {
		// assume finished or idle
		return 1;
	}

	state->last_check = now;
skip:
	return state->bsd_info.pbi_status == SSLEEP || state->bsd_info.pbi_status == SSTOP;
#endif
}

int process_poll_initial_wait(void)
{
	return 128;
}

int process_poll_timeout(bool idle, int timeout)
{
	(void) idle;
	return timeout;
}

int process_poll_backoff(int timeout)
{
	return timeout;
}

#endif // defined(__APPLE__)
