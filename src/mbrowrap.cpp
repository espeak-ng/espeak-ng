/*
 * mbrowrap -- A wrapper library around the mbrola binary
 * providing a subset of the API from the Windows mbrola DLL.
 *
 * Copyright (C) 2010 by Nicolas Pitre <nico@fluxnic.net>
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
 */

#include "speech.h"
#ifdef INCLUDE_MBROLA

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mbrowrap.h"


/*
 * mbrola instance parameters
 */

enum mbr_state {
	MBR_INACTIVE = 0,
	MBR_IDLE,
	MBR_NEWDATA,
	MBR_AUDIO,
	MBR_WEDGED
};

static enum mbr_state mbr_state;

static char *mbr_voice_path;
static int mbr_cmd_fd, mbr_audio_fd, mbr_error_fd, mbr_proc_stat;
static pid_t mbr_pid;
static int mbr_samplerate;
static float mbr_volume = 1.0;
static char mbr_errorbuf[160];

struct datablock {
	struct datablock *next;
	int done;
	int size;
	char buffer[1];  /* 1 or more, dynamically allocated */
};

static struct datablock *mbr_pending_data_head, *mbr_pending_data_tail;

/*
 * Private support code.
 */

static void log(const char *msg, ...)
{
	va_list params;

	va_start(params, msg);
	vfprintf(stderr, msg, params);
	fputc('\n', stderr);
	va_end(params);
}

static void err(const char *errmsg, ...)
{
	va_list params;

	va_start(params, errmsg);
	vsnprintf(mbr_errorbuf, sizeof(mbr_errorbuf), errmsg, params);
	va_end(params);
	log("mbrowrap error: %s", mbr_errorbuf);
}

static int create_pipes(int p1[2], int p2[2], int p3[2])
{
	int error;

	if (pipe(p1) != -1) {
		if (pipe(p2) != -1) {
			if (pipe(p3) != -1) {
				return 0;
			} else
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

static int start_mbrola(const char *voice_path)
{
	int error, p_stdin[2], p_stdout[2], p_stderr[2];
	char charbuf[20];

	if (mbr_state != MBR_INACTIVE) {
		err("mbrola init request when already initialized");
		return -1;
	}

	error = create_pipes(p_stdin, p_stdout, p_stderr);
	if (error)
		return -1;

	mbr_pid = fork();

	if (mbr_pid == -1) {
		error = errno;
		close_pipes(p_stdin, p_stdout, p_stderr);
		err("fork(): %s", strerror(error));
		return -1;
	}

	if (mbr_pid == 0) {
		int i;

		if (dup2(p_stdin[0], 0) == -1 ||
		    dup2(p_stdout[1], 1) == -1 ||
		    dup2(p_stderr[1], 2) == -1) {
			snprintf(mbr_errorbuf, sizeof(mbr_errorbuf),
					"dup2(): %s\n", strerror(errno));
			write(p_stderr[1], mbr_errorbuf, strlen(mbr_errorbuf));
			_exit(1);
		}

		for (i = p_stderr[1]; i > 2; i--)
			close(i);
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);

		snprintf(charbuf, sizeof(charbuf), "%g", mbr_volume);
		execlp("mbrola", "mbrola", "-e", "-v", charbuf,
				voice_path, "-", "-.wav", (char *)NULL);
		/* if execution reaches this point then the exec() failed */
		snprintf(mbr_errorbuf, sizeof(mbr_errorbuf),
				"mbrola: %s\n", strerror(errno));
		write(2, mbr_errorbuf, strlen(mbr_errorbuf));
		_exit(1);
	}

	snprintf(charbuf, sizeof(charbuf), "/proc/%d/stat", mbr_pid);
	mbr_proc_stat = open(charbuf, O_RDONLY);
	if (mbr_proc_stat == -1) {
		error = errno;
		close_pipes(p_stdin, p_stdout, p_stderr);
		waitpid(mbr_pid, NULL, 0);
		mbr_pid = 0;
		err("/proc is unaccessible: %s", strerror(error));
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	if (fcntl(p_stdin[1], F_SETFL, O_NONBLOCK) == -1 ||
	    fcntl(p_stdout[0], F_SETFL, O_NONBLOCK) == -1 ||
	    fcntl(p_stderr[0], F_SETFL, O_NONBLOCK) == -1) {
		error = errno;
		close_pipes(p_stdin, p_stdout, p_stderr);
		waitpid(mbr_pid, NULL, 0);
		mbr_pid = 0;
		err("fcntl(): %s", strerror(error));
		return -1;
	}

	mbr_cmd_fd = p_stdin[1];
	mbr_audio_fd = p_stdout[0];
	mbr_error_fd = p_stderr[0];
	close(p_stdin[0]);
	close(p_stdout[1]);
	close(p_stderr[1]);

	mbr_state = MBR_IDLE;
	return 0;
}

static void stop_mbrola(void)
{
	if (mbr_state == MBR_INACTIVE)
		return;
	close(mbr_proc_stat);
	close(mbr_cmd_fd);
	close(mbr_audio_fd);
	close(mbr_error_fd);
	if (mbr_pid) {
		kill(mbr_pid, SIGTERM);
		waitpid(mbr_pid, NULL, 0);
		mbr_pid = 0;
	}
	mbr_state = MBR_INACTIVE;
}

static void free_pending_data(void)
{
	struct datablock *p, *head = mbr_pending_data_head;
	while (head) {
		p = head;
		head = head->next;
		free(p);
	}
	mbr_pending_data_head = NULL;
	mbr_pending_data_tail = NULL;
}

static int mbrola_died(void)
{
	pid_t pid;
	int status, len;
	const char *msg;
	char msgbuf[80];

	pid = waitpid(mbr_pid, &status, WNOHANG);
	if (!pid) {
		msg = "mbrola closed stderr and did not exit";
	} else if (pid != mbr_pid) {
		msg = "waitpid() is confused";
	} else {
		mbr_pid = 0;
		if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			snprintf(msgbuf, sizeof(msgbuf),
					"mbrola died by signal %d", sig);
			msg = msgbuf;
		} else if (WIFEXITED(status)) {
			int exst = WEXITSTATUS(status);
			snprintf(msgbuf, sizeof(msgbuf),
					"mbrola exited with status %d", exst);
			msg = msgbuf;
		} else {
			msg = "mbrola died and wait status is weird";
		}
	}

	log("mbrowrap error: %s", msg);

	len = strlen(mbr_errorbuf);
	if (!len)
		snprintf(mbr_errorbuf, sizeof(mbr_errorbuf), "%s", msg);
	else
		snprintf(mbr_errorbuf + len, sizeof(mbr_errorbuf) - len,
						", (%s)", msg);
	return -1;
}

static int mbrola_has_errors(void)
{
	int result;
	char buffer[256];
	char *buf_ptr, *lf;

	buf_ptr = buffer;
	for (;;) {
		result = read(mbr_error_fd, buf_ptr,
				sizeof(buffer) - (buf_ptr - buffer) - 1);
		if (result == -1) {
			if (errno == EAGAIN)
				return 0;
			err("read(error): %s", strerror(errno));
			return -1;
		}

		if (result == 0) {
			/* EOF on stderr, assume mbrola died. */
			return mbrola_died();
		}

		buf_ptr[result] = 0;

		for (; (lf = strchr(buf_ptr, '\n')); buf_ptr = lf + 1) {
			/* inhibit the reset signal messages */
			if (strncmp(buf_ptr, "Got a reset signal", 18) == 0 ||
			    strncmp(buf_ptr, "Input Flush Signal", 18) == 0)
				continue;
			*lf = 0;
			log("mbrola: %s", buf_ptr);
			/* is this the last line? */
			if (lf == &buf_ptr[result - 1]) {
				snprintf(mbr_errorbuf, sizeof(mbr_errorbuf),
						"%s", buf_ptr);
				/* don't consider this fatal at this point */
				return 0;
			}
		}

		memmove(buffer, buf_ptr, result);
		buf_ptr = buffer + result;
	}
}

static int send_to_mbrola(const char *cmd)
{
	ssize_t result;
	int len;
	
	if (!mbr_pid)
		return -1;

	len = strlen(cmd);
	result = write(mbr_cmd_fd, cmd, len);

	if (result == -1) {
		int error = errno;
		if (error == EPIPE && mbrola_has_errors()) {
			return -1;
		} else if (error == EAGAIN) {
			result = 0;
		} else {
			err("write(): %s", strerror(error));
			return -1;
		}
	}
	
	if (result != len) {
		struct datablock *data;
		data = (struct datablock *)malloc(sizeof(*data) + len - result);
		if (data) {
			data->next = NULL;
			data->done = 0;
			data->size = len - result;
			memcpy(data->buffer, cmd + result, len - result);
			result = len;
			if (!mbr_pending_data_head)
				mbr_pending_data_head = data;
			else
				mbr_pending_data_tail->next = data;
			mbr_pending_data_tail = data;
		}
	}

	return result;
}

static int mbrola_is_idle(void)
{
	char *p;
	char buffer[20]; /* looking for "12345 (mbrola) S" so 20 is plenty*/

	/* look in /proc to determine if mbrola is still running or sleeping */
	if (lseek(mbr_proc_stat, 0, SEEK_SET) != 0)
		return 0;
	if (read(mbr_proc_stat, buffer, sizeof(buffer)) != sizeof(buffer))
		return 0;
	p = (char *)memchr(buffer, ')', sizeof(buffer));
	if (!p || (unsigned)(p - buffer) >= sizeof(buffer) - 2)
		return 0;
	return (p[1] == ' ' && p[2] == 'S');
}

static ssize_t receive_from_mbrola(void *buffer, size_t bufsize)
{
	int result, wait = 1;
	size_t cursize = 0;

	if (!mbr_pid)
		return -1;

	do {
		struct pollfd pollfd[3];
		nfds_t nfds = 0;
		int idle;

		pollfd[0].fd = mbr_audio_fd;
		pollfd[0].events = POLLIN;
		nfds++;

		pollfd[1].fd = mbr_error_fd;
		pollfd[1].events = POLLIN;
		nfds++;

		if (mbr_pending_data_head) {
			pollfd[2].fd = mbr_cmd_fd;
			pollfd[2].events = POLLOUT;
			nfds++;
		}

		idle = mbrola_is_idle();
		result = poll(pollfd, nfds, idle ? 0 : wait);
		if (result == -1) {
			err("poll(): %s", strerror(errno));
			return -1;
		}
		if (result == 0) {
			if (idle) {
				mbr_state = MBR_IDLE;
				break;
			} else {
				if (wait >= 5000 * (4-1)/4) {
					mbr_state = MBR_WEDGED;
					err("mbrola process is stalled");
					break;
				} else {
					wait *= 4;
					continue;
				}
			}
		}
		wait = 1;

		if (pollfd[1].revents && mbrola_has_errors())
			return -1;

		if (mbr_pending_data_head && pollfd[2].revents) {
			struct datablock *head = mbr_pending_data_head;
			char *data = head->buffer + head->done;
			int left = head->size - head->done;
			result = write(mbr_cmd_fd, data, left);
			if (result == -1) {
				int error = errno;
				if (error == EPIPE && mbrola_has_errors())
					return -1;
				err("write(): %s", strerror(error));
				return -1;
			}
			if (result != left) {
				head->done += result;
			} else {
				mbr_pending_data_head = head->next;
				free(head);
				if (!mbr_pending_data_head)
					mbr_pending_data_tail = NULL;
				else
					continue;
			}
		}

		if (pollfd[0].revents) {
			char *curpos = (char *)buffer + cursize;
			size_t space = bufsize - cursize;
			ssize_t obtained = read(mbr_audio_fd, curpos, space);
			if (obtained == -1) {
				err("read(): %s", strerror(errno));
				return -1;
			}
			cursize += obtained;
			mbr_state = MBR_AUDIO;
		}
	} while (cursize < bufsize);

	return cursize;
}

/*
 * API functions.
 */

int init_MBR(const char *voice_path)
{
	int error, result;
	unsigned char wavhdr[45];

	error = start_mbrola(voice_path);
	if (error)
		return -1;

	result = send_to_mbrola("#\n");
	if (result != 2) {
		stop_mbrola();
		return -1;
	}

	/* we should actually be getting only 44 bytes */
	result = receive_from_mbrola(wavhdr, 45);
	if (result != 44) {
		if (result >= 0)
			err("unable to get .wav header from mbrola");
		stop_mbrola();
		return -1;
	}

	/* parse wavhdr to get mbrola voice samplerate */
	if (memcmp(wavhdr, "RIFF", 4) != 0 ||
	    memcmp(wavhdr+8, "WAVEfmt ", 8) != 0) {
		err("mbrola did not return a .wav header");
		stop_mbrola();
		return -1;
	}
	mbr_samplerate = wavhdr[24] + (wavhdr[25]<<8) +
			 (wavhdr[26]<<16) + (wavhdr[27]<<24);
	//log("mbrowrap: voice samplerate = %d", mbr_samplerate);

	/* remember the voice path for setVolumeRatio_MBR() */
	if (mbr_voice_path != voice_path) {
		free(mbr_voice_path);
		mbr_voice_path = strdup(voice_path);
	} 

	return 0;
}

void close_MBR(void)
{
	stop_mbrola();
	free_pending_data();
	free(mbr_voice_path);
	mbr_voice_path = NULL;
	mbr_volume = 1.0;
}

int reset_MBR()
{
	int result, success = 1;
	char dummybuf[4096];

	if (mbr_state == MBR_IDLE)
		return 1;
	if (!mbr_pid)
		return 0;
	if (kill(mbr_pid, SIGUSR1) == -1)
		success = 0;
	free_pending_data();
	result = write(mbr_cmd_fd, "\n#\n", 3);
	if (result != 3)
		success = 0;
	do {
		result = read(mbr_audio_fd, dummybuf, sizeof(dummybuf));
	} while (result > 0);
	if (result != -1 || errno != EAGAIN)
		success = 0;
	if (!mbrola_has_errors() && success)
		mbr_state = MBR_IDLE;
	return success;
}

int read_MBR(void *buffer, int nb_samples)
{
	int result = receive_from_mbrola(buffer, nb_samples * 2);
	if (result > 0)
		result /= 2;
	return result;
}

int write_MBR(const char *data)
{
	mbr_state = MBR_NEWDATA;
	return send_to_mbrola(data);
}

int flush_MBR(void)
{
	return send_to_mbrola("\n#\n") == 3;
}

int getFreq_MBR(void)
{
	return mbr_samplerate;
}

void setVolumeRatio_MBR(float value)
{
	if (value == mbr_volume)
		return;
	mbr_volume = value;
	if (mbr_state != MBR_IDLE)
		return;
	/*
	 * We have no choice but to kill and restart mbrola with
	 * the new argument here.
	 */
	stop_mbrola();
	init_MBR(mbr_voice_path);
}

int lastErrorStr_MBR(char *buffer, int bufsize)
{
	int result;
	if (mbr_pid)
		mbrola_has_errors();
	result = snprintf(buffer, bufsize, "%s", mbr_errorbuf);
	return result >= bufsize ? (bufsize - 1) : result;
}

void resetError_MBR(void)
{
	mbr_errorbuf[0] = 0;
}

#endif  // INCLUDE_MBROLA
