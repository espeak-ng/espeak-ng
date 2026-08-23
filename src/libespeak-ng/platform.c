#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "platform.h"

#define err(fmt, ...) fprintf(stderr, "platform error: " fmt "\n", ##__VA_ARGS__)

static int create_pipes(int p1[2], int p2[2], int p3[2])
{
	int error;

	if (pipe(p1) != -1) {
		if (pipe(p2) != -1) {
			if (pipe(p3) != -1)
				return 0;
			else
				error = errno;
			close(p2[0]);
			close(p2[1]);
		} else
			error = errno;
		close(p1[0]);
		close(p1[1]);
	} else
		error = errno;

	err("pipe(): %s", strerror(error));
	return -1;
}

static void close_pipes(int p1[2], int p2[2], int p3[2])
{
	close(p1[0]);
	close(p1[1]);
	close(p2[0]);
	close(p2[1]);
	close(p3[0]);
	close(p3[1]);
}

int process_start(struct process *process, const char *executable, char *const argv[])
{
	int error, p_stdin[2], p_stdout[2], p_stderr[2];
	ssize_t written;

	error = create_pipes(p_stdin, p_stdout, p_stderr);
	if (error)
		return -1;

	process->pid = fork();
	if (process->pid == -1) {
		error = errno;
		close_pipes(p_stdin, p_stdout, p_stderr);
		err("fork(): %s", strerror(error));
		return -1;
	}

	char mbr_errorbuf[160];

	if (process->pid == 0) {
		int i;

		if (dup2(p_stdin[0], 0) == -1 ||
		    dup2(p_stdout[1], 1) == -1 ||
		    dup2(p_stderr[1], 2) == -1) {
			snprintf(mbr_errorbuf, sizeof(mbr_errorbuf),
			         "dup2(): %s\n", strerror(errno));
			written = write(p_stderr[1], mbr_errorbuf, strlen(mbr_errorbuf));
			(void)written;   // suppress 'variable not used' warning
			_exit(1);
		}

		for (i = p_stderr[1]; i > 2; i--)
			close(i);

		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);

		execvp(executable, argv);

		/* if execution reaches this point then the exec() failed */
		snprintf(mbr_errorbuf, sizeof(mbr_errorbuf),
		         "mbrola: %s\n", strerror(errno));
		written = write(2, mbr_errorbuf, strlen(mbr_errorbuf));
		(void)written;   // suppress 'variable not used' warning
		_exit(1);
	}

	error = process_start_monitor(process);
	if (error) {
		err("process_start_monitor failed with error: %d", error);
		close_pipes(p_stdin, p_stdout, p_stderr);
		waitpid(process->pid, NULL, 0);
		process->pid = 0;
		return error;
	}

	signal(SIGPIPE, SIG_IGN);

	if (fcntl(p_stdin[1], F_SETFL, O_NONBLOCK) == -1 ||
	    fcntl(p_stdout[0], F_SETFL, O_NONBLOCK) == -1 ||
	    fcntl(p_stderr[0], F_SETFL, O_NONBLOCK) == -1) {
		error = errno;
		close_pipes(p_stdin, p_stdout, p_stderr);
		waitpid(process->pid, NULL, 0);
		process->pid = 0;
		err("fcntl(): %s", strerror(error));
		return -1;
	}

	process->stdin = p_stdin[1];
	process->stdout = p_stdout[0];
	process->stderr = p_stderr[0];
	close(p_stdin[0]);
	close(p_stdout[1]);
	close(p_stderr[1]);

	return 0;
}

void process_stop(struct process *process)
{
	process_stop_monitor(process);

	close(process->stdin);
	process->stdin = -1;

	close(process->stdout);
	process->stdout = -1;

	close(process->stderr);
	process->stderr = -1;

	if (process->pid) {
		kill(process->pid, SIGTERM);
		waitpid(process->pid, NULL, 0);
		process->pid = 0;
	}
}
