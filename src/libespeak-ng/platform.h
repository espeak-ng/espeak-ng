#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <sys/types.h>

#if defined(__SANITIZE_THREAD__)
	#define HAS_TSAN
#elif defined(__has_feature)
	#if __has_feature(thread_sanitizer)
		#define HAS_TSAN
	#endif
#endif


#ifdef __cplusplus
extern "C"
{
#endif

struct process {
	pid_t pid;
	/** mbr_cmd_fd */
	int stdin;
	/** mbr_audio_fd */
	int stdout;
	/** mbr_err_fd */
	int stderr;

	void *state;
};


int process_start(struct process *process, const char *executable, char *const argv[]);
void process_stop(struct process *process);

int process_start_monitor(struct process *process);
void process_stop_monitor(struct process *process);

/**
 * Queries whether the process is idle, to decide if we are ready to skip the poll timeout.
 *
 * It should be a fast operation ideally without context switching.
 */
bool process_is_idle(struct process *process);

/** in milliseconds */
int process_poll_initial_wait(void);
int process_poll_timeout(bool idle, int timeout);
int process_poll_backoff(int timeout);

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_H
