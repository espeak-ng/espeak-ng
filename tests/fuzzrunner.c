/*
 * Copyright (C) 2018 Sascha Brawer
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

#undef NDEBUG
#include "config.h"
#include "common.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <espeak-ng/espeak_ng.h>

extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int main(int argc, char **argv) {
	int i;

	for (i = 1; i < argc; i++) {
		size_t filesize = GetFileLength(argv[i]);
		FILE *stream = fopen(argv[i], "r");
		unsigned char *text = NULL;
		if (stream == NULL) {
			perror(argv[i]);
			exit(EXIT_FAILURE);
		}

		text = (unsigned char *) malloc(filesize + 1);
		if (text == NULL) {
			espeak_ng_PrintStatusCodeMessage(ENOMEM, stderr, NULL);
			exit(EXIT_FAILURE);
		}

		fread(text, 1, filesize, stream);
		text[filesize] = 0;
		fclose(stream);

		LLVMFuzzerTestOneInput(text, filesize);
		free(text);
	}

	return EXIT_SUCCESS;
}
