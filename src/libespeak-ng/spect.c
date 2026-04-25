/*
 * Copyright (C) 2005 to 2007 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2016 Reece H. Dunn
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
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include "spect.h"
#include "ieee80.h"                    // for ConvertFromIeeeExtended
#include "wavegen.h"                   // for wavegen_peaks_t, PeaksToHarmspect
#include "synthesize.h"                // for KLATT_AV, KLATT_Kopen, N_KLATTP2
#include "voice.h"                     // for N_PEAKS

static const int default_freq[N_PEAKS] =
{ 200, 500, 1200, 3000, 3500, 4000, 6900, 7800, 9000 };
static const int default_width[N_PEAKS] =
{ 750, 500, 550, 550, 600, 700, 700, 700, 700 };
static const int default_klt_bw[N_PEAKS] =
{ 89, 90, 140, 260, 260, 260, 500, 500, 500 };

static espeak_ng_STATUS read_double(FILE *stream, double *out)
{
	unsigned char bytes[10];
	if (fread(bytes, sizeof(char), 10, stream) != 10)
		return ENS_UNEXPECTED_EOF;
	*out = ieee_extended_to_double(bytes);
	return ENS_OK;
}

float polint(float xa[], float ya[], int n, float x)
{
	// General polinomial interpolation routine, xa[1...n] ya[1...n]
	int i, m, ns = 1;
	float den, dif, dift, ho, hp, w;
	float y; // result
	float c[9], d[9];

	dif = fabs(x-xa[0]);

	for (i = 1; i <= n; i++) {
		if ((dift = fabs(x-xa[i-1])) < dif) {
			ns = i;
			dif = dift;
		}
		c[i] = ya[i-1];
		d[i] = ya[i-1];
	}
	y = ya[--ns];
	for (m = 1; m < n; m++) {
		for (i = 1; i <= n-m; i++) {
			ho = xa[i-1]-x;
			hp = xa[i+m-1]-x;
			w = c[i+1]-d[i];
			if ((den = ho-hp) == 0.0)
				return ya[1]; // two input xa are identical
			den = w/den;
			d[i] = hp*den;
			c[i] = ho*den;
		}
		y += ((2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	return y;
}

static SpectFrame *SpectFrameCreate(void)
{
	int ix;
	SpectFrame *frame;

	frame = malloc(sizeof(SpectFrame));
	if (!frame)
		return NULL;

	frame->keyframe = 0;
	frame->spect = NULL;
	frame->markers = 0;
	frame->pitch = 0;
	frame->nx = 0;
	frame->time = 0;
	frame->length = 0;
	frame->amp_adjust = 100;
	frame->length_adjust = 0;

	for (ix = 0; ix < N_PEAKS; ix++) {
		frame->formants[ix].freq = 0;
		frame->peaks[ix].pkfreq = default_freq[ix];
		frame->peaks[ix].pkheight = 0;
		frame->peaks[ix].pkwidth = default_width[ix];
		frame->peaks[ix].pkright = default_width[ix];
		frame->peaks[ix].klt_bw = default_klt_bw[ix];
		frame->peaks[ix].klt_ap = 0;
		frame->peaks[ix].klt_bp = default_klt_bw[ix];
	}

	memset(frame->klatt_param, 0, sizeof(frame->klatt_param));
	frame->klatt_param[KLATT_AV] = 59;
	frame->klatt_param[KLATT_Kopen] = 40;

	return frame;
}

static void SpectFrameDestroy(SpectFrame *frame)
{
	if (frame->spect != NULL)
		free(frame->spect);
	free(frame);
}

/* Read exactly one short from stream, byte-swap it, and store it.
 * Returns ENS_UNEXPECTED_EOF if the read is short. */
#define READ_SHORT(dest, stream) \
	do { \
		if (fread(&(dest), sizeof(short), 1, (stream)) != 1) \
			return ENS_UNEXPECTED_EOF; \
		(dest) = le16toh(dest); \
	} while (0)

static espeak_ng_STATUS LoadFrame(SpectFrame *frame, FILE *stream, int file_format_type)
{
	short ix;
	short x;
	unsigned short *spect_data;
	espeak_ng_STATUS status;
	double tmp;

	if ((status = read_double(stream, &tmp)) != ENS_OK) return status;
	frame->time = (float)tmp;
	if ((status = read_double(stream, &tmp)) != ENS_OK) return status;
	frame->pitch = (float)tmp;
	if ((status = read_double(stream, &tmp)) != ENS_OK) return status;
	frame->length = (float)tmp;
	if ((status = read_double(stream, &tmp)) != ENS_OK) return status;
	frame->dx = (float)tmp;

	READ_SHORT(frame->nx,         stream);
	READ_SHORT(frame->markers,    stream);
	READ_SHORT(frame->amp_adjust, stream);

	if (file_format_type == 2) {
		READ_SHORT(ix, stream); // spare
		READ_SHORT(ix, stream); // spare
	}

	for (ix = 0; ix < N_PEAKS; ix++) {
		READ_SHORT(frame->formants[ix].freq,    stream);
		READ_SHORT(frame->formants[ix].bandw,   stream);
		READ_SHORT(frame->peaks[ix].pkfreq,     stream);
		READ_SHORT(frame->peaks[ix].pkheight,   stream);
		READ_SHORT(frame->peaks[ix].pkwidth,    stream);
		READ_SHORT(frame->peaks[ix].pkright,    stream);
		if (frame->peaks[ix].pkheight > 0)
			frame->keyframe = 1;

		if (file_format_type == 2) {
			READ_SHORT(frame->peaks[ix].klt_bw, stream);
			READ_SHORT(frame->peaks[ix].klt_ap, stream);
			READ_SHORT(frame->peaks[ix].klt_bp, stream);
		}
	}

	if (file_format_type > 0) {
		for (ix = 0; ix < N_KLATTP2; ix++)
			READ_SHORT(frame->klatt_param[ix], stream);
	}

	spect_data = malloc(sizeof(unsigned short) * frame->nx);
	if (spect_data == NULL)
		return ENOMEM;

	frame->max_y = 0;
	for (ix = 0; ix < frame->nx; ix++) {
		if (fread(&x, sizeof(short), 1, stream) != 1) {
			free(spect_data);
			return ENS_UNEXPECTED_EOF;
		}
		x = le16toh(x);
		spect_data[ix] = x;
		if (x > frame->max_y) frame->max_y = x;
	}
	frame->spect = spect_data;

	return ENS_OK;
}

#undef READ_SHORT

double GetFrameRms(SpectFrame *frame, int seq_amplitude)
{
	int h;
	float total = 0;
	int maxh;
	int height;
	int htab[400];
	wavegen_peaks_t wpeaks[9];

	for (h = 0; h < 9; h++) {
		height = (frame->peaks[h].pkheight * seq_amplitude * frame->amp_adjust)/10000;
		wpeaks[h].height = height << 8;

		wpeaks[h].freq = frame->peaks[h].pkfreq << 16;
		wpeaks[h].left = frame->peaks[h].pkwidth << 16;
		wpeaks[h].right = frame->peaks[h].pkright << 16;
	}

	maxh = PeaksToHarmspect(wpeaks, 90<<16, htab, 0);
	for (h = 1; h < maxh; h++)
		total += ((htab[h] * htab[h]) >> 10);
	frame->rms = sqrt(total) / 7.25;
	return frame->rms;
}

#pragma GCC visibility push(default)
SpectSeq *SpectSeqCreate(void)
{
	SpectSeq *spect = malloc(sizeof(SpectSeq));
	if (!spect)
		return NULL;

	spect->numframes = 0;
	spect->frames = NULL;
	spect->name = NULL;

	spect->grid = 1;
	spect->duration = 0;
	spect->pitch1 = 0;
	spect->pitch2 = 0;
	spect->bass_reduction = 0;

	spect->max_x = 3000;
	spect->max_y = 1;
	spect->file_format = 0;

	return spect;
}

void SpectSeqDestroy(SpectSeq *spect)
{
	int ix;
	if (spect->frames != NULL) {
		for (ix = 0; ix < spect->numframes; ix++) {
			if (spect->frames[ix] != NULL)
				SpectFrameDestroy(spect->frames[ix]);
		}
		free(spect->frames);
	}
	free(spect->name);
	free(spect);
}
#pragma GCC visibility pop

static float GetFrameLength(SpectSeq *spect, int frame)
{
	int ix;
	float adjust = 0;

	if (frame >= spect->numframes-1) return 0;

	for (ix = frame+1; ix < spect->numframes-1; ix++) {
		if (spect->frames[ix]->keyframe)
			break; // reached next keyframe
		adjust += spect->frames[ix]->length_adjust;
	}
	return (spect->frames[ix]->time - spect->frames[frame]->time) * 1000.0 + adjust;
}

/* Read and byte-swap a uint32 from stream. Jumps to eof_error label on failure. */
#define READ_U32(dest, stream) \
	do { \
		if (fread(&(dest), sizeof(uint32_t), 1, (stream)) != 1) \
			goto eof_error; \
		(dest) = le32toh(dest); \
	} while (0)

/* Read and byte-swap a short from stream. Jumps to eof_error label on failure. */
#define READ_S16(dest, stream) \
	do { \
		if (fread(&(dest), sizeof(short), 1, (stream)) != 1) \
			goto eof_error; \
		(dest) = le16toh(dest); \
	} while (0)

#pragma GCC visibility push(default)
espeak_ng_STATUS LoadSpectSeq(SpectSeq *spect, const char *filename)
{
	short n, temp;
	int ix;
	uint32_t id1, id2, name_len;
	int set_max_y = 0;
	float time_offset;

	FILE *stream = fopen(filename, "rb");
	if (stream == NULL) {
		fprintf(stderr, "Failed to open: '%s'", filename);
		return errno;
	}

	READ_U32(id1, stream);
	READ_U32(id2, stream);

	if ((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSEQ))
		spect->file_format = 0; // eSpeak formants
	else if ((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSEK))
		spect->file_format = 1; // formants for Klatt synthesizer
	else if ((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSQ2))
		spect->file_format = 2; // formants for Klatt synthesizer
	else {
		fprintf(stderr, "Unsupported spectral file format.\n");
		fclose(stream);
		return ENS_UNSUPPORTED_PHON_FORMAT;
	}

	READ_U32(name_len, stream);
	if (name_len > 0) {
		if ((spect->name = (char *)malloc(name_len)) == NULL) {
			fclose(stream);
			return ENOMEM;
		}
		if (fread(spect->name, sizeof(char), name_len, stream) != name_len)
			goto eof_error;
	} else
		spect->name = NULL;

	READ_S16(n,               stream);
	READ_S16(spect->amplitude, stream);
	READ_S16(spect->max_y,    stream);
	READ_S16(temp,            stream); // unused

	if (n == 0) {
		fclose(stream);
		return ENS_NO_SPECT_FRAMES;
	}

	if (spect->frames != NULL) {
		for (ix = 0; ix < spect->numframes; ix++) {
			if (spect->frames[ix] != NULL)
				SpectFrameDestroy(spect->frames[ix]);
		}
		free(spect->frames);
	}
	spect->frames = calloc(n, sizeof(SpectFrame *));

	spect->numframes = 0;
	spect->max_x = 3000;
	if (spect->max_y == 0) {
		set_max_y = 1;
		spect->max_y = 1;
	}
	for (ix = 0; ix < n; ix++) {
		SpectFrame *frame = SpectFrameCreate();
		if (!frame) {
			fclose(stream);
			return ENOMEM;
		}

		espeak_ng_STATUS status = LoadFrame(frame, stream, spect->file_format);
		if (status != ENS_OK) {
			free(frame);
			fclose(stream);
			return status;
		}

		spect->frames[spect->numframes++] = frame;

		if (set_max_y && (frame->max_y > spect->max_y))
			spect->max_y = frame->max_y;
		if (frame->nx * frame->dx > spect->max_x) spect->max_x = (int)(frame->nx * frame->dx);
	}
	spect->max_x = 9000; // disable auto-xscaling

	// start times from zero
	time_offset = spect->frames[0]->time;
	for (ix = 0; ix < spect->numframes; ix++)
		spect->frames[ix]->time -= time_offset;

	spect->pitch1 = spect->pitchenv.pitch1;
	spect->pitch2 = spect->pitchenv.pitch2;
	spect->duration = (int)(spect->frames[spect->numframes-1]->time * 1000);

	if (spect->max_y < 400)
		spect->max_y = 200;
	else
		spect->max_y = 29000; // disable auto height scaling

	for (ix = 0; ix < spect->numframes; ix++) {
		if (spect->frames[ix]->keyframe)
			spect->frames[ix]->length_adjust = spect->frames[ix]->length - GetFrameLength(spect, ix);
	}
	fclose(stream);
	return ENS_OK;

eof_error:
	fclose(stream);
	return ENS_UNEXPECTED_EOF;
}
#pragma GCC visibility pop

#undef READ_U32
#undef READ_S16
