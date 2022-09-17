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

#ifndef ESPEAK_NG_SPECT_H
#define ESPEAK_NG_SPECT_H

#include <espeak-ng/espeak_ng.h>     // for espeak_ng_STATUS
#include "voice.h"                   // for N_PEAKS
#include "synthesize.h"              // for N_KLATTP2

#ifdef __cplusplus
extern "C"
{
#endif

float polint(float xa[], float ya[], int n, float x);

#define FRAME_WIDTH  1000  // max width for 8000kHz frame
#define MAX_DISPLAY_FREQ 9500

#define T_AMPLITUDE 308
#define T_AV      312
#define T_FNZ     321

#define FILEID1_SPECTSEQ 0x43455053
#define FILEID2_SPECTSEQ 0x51455354  // for eSpeak sequence
#define FILEID2_SPECTSEK 0x4b455354  // for Klatt sequence
#define FILEID2_SPECTSQ2 0x32515354  // with Klatt data

typedef struct {
	unsigned short pitch1;
	unsigned short pitch2;
	unsigned char env[128];
} PitchEnvelope;

typedef struct {
	short freq;
	short bandw;
} formant_t;

typedef struct {
	short pkfreq;
	short pkheight;
	short pkwidth;
	short pkright;
	short klt_bw;
	short klt_ap;
	short klt_bp;
} peak_t;

typedef struct {
	int keyframe;
	short amp_adjust;
	float length_adjust;
	double rms;

	float time;
	float pitch;
	float length;
	float dx;
	unsigned short nx;
	short markers;
	int max_y;
	unsigned short *spect; // sqrt of harmonic amplitudes,  1-nx at 'pitch'

	short klatt_param[N_KLATTP2];

	formant_t formants[N_PEAKS]; // this is just the estimate given by Praat
	peak_t peaks[N_PEAKS];
} SpectFrame;

double GetFrameRms(SpectFrame *frame, int amp);

typedef struct {
	int numframes;
	short amplitude;
	int spare;
	char *name;

	SpectFrame **frames;
	PitchEnvelope pitchenv;
	int pitch1;
	int pitch2;
	int duration;
	int grid;
	int bass_reduction;
	int max_x;
	short max_y;
	int file_format;
} SpectSeq;

SpectSeq *SpectSeqCreate(void);
void SpectSeqDestroy(SpectSeq *spect);
espeak_ng_STATUS LoadSpectSeq(SpectSeq *spect, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
