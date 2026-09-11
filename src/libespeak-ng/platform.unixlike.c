#if !defined(__APPLE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(__sun) && defined(__SVR4)
#include <procfs.h>
#endif

#include "platform.h"

struct process_state {
	int mbr_proc_stat;
};

int process_start_monitor(struct process *process)
{
	struct process_state *state = calloc(1, sizeof(struct process_state));
	if (!state)
		return -1;

	char charbuf[20];
#if defined(__sun) && defined(__SVR4)
	snprintf(charbuf, sizeof(charbuf), "/proc/%d/psinfo", process->pid);
#else
	snprintf(charbuf, sizeof(charbuf), "/proc/%d/stat", process->pid);
#endif
	state->mbr_proc_stat = open(charbuf, O_RDONLY);
	if (state->mbr_proc_stat == -1) {
		int error = errno;
		err("/proc is unaccessible: %s", strerror(error));
		return -1;
	}

	process->state = state;

	return 0;
}

void process_stop_monitor(struct process *process)
{
	struct process_state *state = process->state;

	close(state->mbr_proc_stat);
	state->mbr_proc_stat = -1;

	free(state);
	process->state = NULL;
}

bool process_is_idle(struct process *process)
{
	struct process_state *state = process->state;

#if defined(__sun) && defined(__SVR4)
	psinfo_t ps;

	if (pread(state->mbr_proc_stat, &ps, sizeof(ps), 0) != sizeof(ps))
		return false;

	return strcmp(ps.pr_fname, "mbrola") == 0 && ps.pr_lwp.pr_sname == 'S';
#else
	char *p;
	char buffer[20]; // looking for "12345 (mbrola) S" so 20 is plenty

	// look in /proc to determine if mbrola is still running or sleeping
	if (lseek(state->mbr_proc_stat, 0, SEEK_SET) != 0)
		return false;
	if (read(state->mbr_proc_stat, buffer, sizeof(buffer)) != sizeof(buffer))
		return false;
	p = (char *)memchr(buffer, ')', sizeof(buffer));
	if (!p || (unsigned)(p - buffer) >= sizeof(buffer) - 2)
		return false;
	return p[1] == ' ' && p[2] == 'S';
#endif
}

int process_poll_initial_wait(void)
{
	return 1;
}

int process_poll_timeout(bool idle, int timeout)
{
	return idle ? 0 : timeout;
}

int process_poll_backoff(int timeout)
{
	return timeout * 4;
}

#endif // !defined(__APPLE__)
