/*
 * Copyright (C) 2026 espeak-ng contributors
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
 * along with this program; if not, write see:
 *             <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <dirent.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <espeak-ng/espeak_ng.h>

#define PATH_BUF 1024
#define MAX_INPUT_SIZE (512 * 1024)

extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);
extern int LLVMFuzzerInitialize(const int *argc, char ***argv);

static FILE *f_log = NULL;
static char dsource[PATH_BUF];
static char fname_rules[PATH_BUF];
static const char *workdir = NULL;

static void fatal(const char *what)
{
	fprintf(stderr, "dict-fuzzer: %s failed\n", what);
	abort();
}

static void mirror_data_dir(const char *srcdir, const char *dstdir)
{
	DIR *dir;
	struct dirent *ent;

	dir = opendir(srcdir);
	if (dir == NULL)
		fatal("opendir(espeak-ng-data)");

	if (mkdir(dstdir, 0700) != 0)
		fatal("mkdir(espeak-ng-data mirror)");

	while ((ent = readdir(dir)) != NULL) {
		char from[PATH_BUF];
		char to[PATH_BUF];

		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;

		snprintf(from, sizeof(from), "%s/%s", srcdir, ent->d_name);
		snprintf(to, sizeof(to), "%s/%s", dstdir, ent->d_name);
		if (symlink(from, to) != 0)
			fatal("symlink(espeak-ng-data entry)");
	}

	closedir(dir);
}

static void remove_workdir(void)
{
	char path[PATH_BUF];
	struct dirent *ent;
	DIR *dir;

	snprintf(path, sizeof(path), "%s/espeak-ng-data", workdir);

	dir = opendir(path);
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;

			unlinkat(dirfd(dir), ent->d_name, 0);
		}
		closedir(dir);
	}

	rmdir(path);
	unlink(fname_rules);
	rmdir(workdir);
}

int LLVMFuzzerInitialize(const int *argc, char ***argv)
{
	static char tmpdir[] = "/tmp/espeak-dict-fuzzXXXXXX";
	const char *datapath;
	char srcdir[PATH_BUF];
	char dstdir[PATH_BUF];

	(void)argc;

	datapath = getenv("ESPEAK_DATA_PATH");
	if (datapath == NULL)
		datapath = dirname(strdup((*argv)[0]));

	if (mkdtemp(tmpdir) == NULL)
		fatal("mkdtemp");

	snprintf(srcdir, sizeof(srcdir), "%s/espeak-ng-data", datapath);
	snprintf(dstdir, sizeof(dstdir), "%s/espeak-ng-data", tmpdir);
	mirror_data_dir(srcdir, dstdir);

	espeak_ng_InitializePath(tmpdir);

	if (espeak_ng_Initialize(NULL) != ENS_OK)
		fatal("espeak_ng_Initialize");
	if (espeak_ng_SetVoiceByName("en") != ENS_OK)
		fatal("espeak_ng_SetVoiceByName(\"en\")");

	f_log = fopen("/dev/null", "w");
	if (f_log == NULL)
		fatal("fopen(/dev/null)");

	snprintf(dsource, sizeof(dsource), "%s/", tmpdir);
	snprintf(fname_rules, sizeof(fname_rules), "%s/fz_rules", tmpdir);

	workdir = tmpdir;
	atexit(remove_workdir);

	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FILE *fp;

	if (size == 0 || size > MAX_INPUT_SIZE)
		return 0;

	fp = fopen(fname_rules, "wb");
	if (fp == NULL)
		fatal("fopen(fz_rules)");
	if (fwrite(data, 1, size, fp) != size) {
		fclose(fp);
		fatal("fwrite(fz_rules)");
	}
	fclose(fp);

	espeak_ng_CompileDictionary(dsource, "fz", f_log, 0, NULL);

	return 0;
}
