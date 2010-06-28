/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/



typedef struct {
	char v_name[40];

	int phoneme_tab_ix;  // phoneme table number
	int pitch_base;    // Hz<<12
	int pitch_range;   // standard = 0x1000
	
	int speedf1;
	int speedf2;
	int speedf3;

	int speed_percent;      // adjust the WPM speed by this percentage
	int flutter;
	int roughness;
	int echo_delay;
	int echo_amp;
	int n_harmonic_peaks;  // highest formant which is formed from adding harmonics
	int peak_shape;        // alternative shape for formant peaks (0=standard 1=squarer)
	int voicing;           // 100% = 64, level of formant-synthesized sound
	int formant_factor;      // adjust nominal formant frequencies by this  because of the voice's pitch (256ths)
	int consonant_amp;     // amplitude of unvoiced consonants
	int consonant_ampv;    // amplitude of the noise component of voiced consonants
	int samplerate;
	int klattv[8];

	// parameters used by Wavegen
	short freq[N_PEAKS];    // 100% = 256
	short height[N_PEAKS];  // 100% = 256
	short width[N_PEAKS];   // 100% = 256
	short freqadd[N_PEAKS];  // Hz

	// copies without temporary adjustments from embedded commands
	short freq2[N_PEAKS];    // 100% = 256
	short height2[N_PEAKS];  // 100% = 256
	short width2[N_PEAKS];   // 100% = 256

	int breath[N_PEAKS];  // amount of breath for each formant. breath[0] indicates whether any are set.
	int breathw[N_PEAKS];  // width of each breath formant

	// This table provides the opportunity for tone control.
	// Adjustment of harmonic amplitudes, steps of 8Hz
	// value of 128 means no change
	#define N_TONE_ADJUST  1000
	unsigned char tone_adjust[N_TONE_ADJUST];   //  8Hz steps * 1000 = 8kHz

} voice_t;

// percentages shown to user, ix=N_PEAKS means ALL peaks
extern USHORT voice_pcnt[N_PEAKS+1][3];


extern voice_t *voice;
extern int tone_points[12];

const char *SelectVoice(espeak_VOICE *voice_select, int *found);
espeak_VOICE *SelectVoiceByName(espeak_VOICE **voices, const char *name);
voice_t *LoadVoice(const char *voice_name, int control);
voice_t *LoadVoiceVariant(const char *voice_name, int variant);
void DoVoiceChange(voice_t *v);
void WVoiceChanged(voice_t *wvoice);
void WavegenSetVoice(voice_t *v);
void ReadTonePoints(char *string, int *tone_pts);
void VoiceReset(int control);

