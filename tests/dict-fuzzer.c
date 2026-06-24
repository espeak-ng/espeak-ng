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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include <espeak-ng/espeak_ng.h>

static int initialized = 0;
static int ready = 0;
char *filepath = NULL;

extern int LLVMFuzzerInitialize(const int *argc, char ***argv);
extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerInitialize(const int *argc, char ***argv) {
	(void)argc;
	filepath = dirname(strdup((*argv)[0]));
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	if (size == 0 || size > 65536) {
		return 0;
	}

	if (!initialized) {
		initialized = 1;
		if (!getenv("ESPEAK_DATA_PATH")) {
			setenv("ESPEAK_DATA_PATH", filepath, 0);
		}
		espeak_ng_InitializePath(getenv("ESPEAK_DATA_PATH"));
		if (espeak_ng_Initialize(NULL) == ENS_OK &&
		    espeak_ng_SetVoiceByName("en") == ENS_OK) {
			ready = 1;
		}
	}
	if (!ready) {
		return 0;
	}

	char dir[] = "/tmp/edictXXXXXX";
	if (mkdtemp(dir) == NULL) {
		return 0;
	}

	char rules[300];
	snprintf(rules, sizeof(rules), "%s/fz_rules", dir);

	FILE *fp = fopen(rules, "wb");
	if (fp != NULL) {
		fwrite(data, 1, size, fp);
		fclose(fp);

		char dsource[300];
		snprintf(dsource, sizeof(dsource), "%s/", dir);
		espeak_ng_CompileDictionary(dsource, "fz", NULL, 0, NULL);

		unlink(rules);
	}

	rmdir(dir);
	return 0;
}
