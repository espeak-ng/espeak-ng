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


#define N_PHONEME_LIST  1000    // enough for source[N_TR_SOURCE] full of text, else it will truncate

#define MAX_HARMONIC  400           // 400 * 50Hz = 20 kHz, more than enough
#define N_SEQ_FRAMES   25           // max frames in a spectrum sequence (real max is ablut 8)
#define STEPSIZE  64                // 2.9mS at 22 kHz sample rate

#define    PITCHfall   0
#define    PITCHrise   1

// flags set for frames within a spectrum sequence
#define FRFLAG_KLATT           0x01   // this frame includes extra data for Klatt synthesizer
#define FRFLAG_VOWEL_CENTRE    0x02   // centre point of vowel
#define FRFLAG_LEN_MOD         0x04   // reduce effect of length adjustment
#define FRFLAG_BREAK_LF        0x08   // but keep f3 upwards
#define FRFLAG_BREAK           0x10   // don't merge with next frame
#define FRFLAG_BREAK_2         0x18   // FRFLAG_BREAK_LF or FRFLAG_BREAK
#define FRFLAG_FORMANT_RATE    0x20   // Flag5 allow increased rate of change of formant freq
#define FRFLAG_MODULATE        0x40   // Flag6 modulate amplitude of some cycles to give trill
#define FRFLAG_DEFER_WAV       0x80   // Flag7 defer mixing WAV until the next frame
#define FRFLAG_COPIED        0x8000   // This frame has been copied into temporary rw memory

#define SFLAG_SEQCONTINUE      0x01   // a liquid or nasal after a vowel, but not followed by a vowel
#define SFLAG_EMBEDDED         0x02   // there are embedded commands before this phoneme
#define SFLAG_SYLLABLE         0x04   // vowel or syllabic consonant
#define SFLAG_LENGTHEN         0x08   // lengthen symbol : included after this phoneme
#define SFLAG_DICTIONARY       0x10   // the pronunciation of this word was listed in the xx_list dictionary
#define SFLAG_SWITCHED_LANG    0x20   // this word uses phonemes from a different language
#define SFLAG_PROMOTE_STRESS   0x40   // this unstressed word can be promoted to stressed

// embedded command numbers
#define EMBED_P     1   // pitch
#define EMBED_S     2   // speed (used in setlengths)
#define EMBED_A     3   // amplitude/volume
#define EMBED_R     4   // pitch range/expression
#define EMBED_H     5   // echo/reverberation
#define EMBED_T     6   // different tone for announcing punctuation
#define EMBED_I     7   // sound icon
#define EMBED_S2    8   // speed (used in synthesize)
#define EMBED_Y     9   // say-as commands
#define EMBED_M    10   // mark name
#define EMBED_U    11   // audio uri
#define EMBED_B    12   // break
#define EMBED_F    13   // emphasis

#define N_EMBEDDED_VALUES    14
extern int embedded_value[N_EMBEDDED_VALUES];
extern int embedded_default[N_EMBEDDED_VALUES];


#define N_PEAKS   9
#define N_MARKERS 8

#define N_KLATTP   10   // this affects the phoneme data file format
#define N_KLATTP2  14   // used in vowel files, with extra parameters for future extensions

#define KLATT_AV      0
#define KLATT_FNZ     1    // nasal zero freq
#define KLATT_Tilt    2
#define KLATT_Aspr    3
#define KLATT_Skew    4

#define KLATT_Kopen   5
#define KLATT_AVp     6
#define KLATT_Fric    7
#define KLATT_FricBP  8
#define KLATT_Turb    9



typedef struct {  // 44 bytes
	short frflags;
	short ffreq[7];
	unsigned char length;
	unsigned char rms;
	unsigned char fheight[8];
	unsigned char fwidth[6];          // width/4  f0-5
	unsigned char fright[3];          // width/4  f0-2
	unsigned char bw[4];        // Klatt bandwidth BNZ /2, f1,f2,f3
	unsigned char klattp[5];    // AV, FNZ, Tilt, Aspr, Skew
	unsigned char klattp2[5];   // continuation of klattp[],  Avp, Fric, FricBP, Turb
	unsigned char klatt_ap[7];  // Klatt parallel amplitude
	unsigned char klatt_bp[7];  // Klatt parallel bandwidth  /2
} frame_t;   //  with extra Klatt parameters for parallel resonators

typedef struct {  // 44 bytes
	short frflags;
	short ffreq[7];
	unsigned char length;
	unsigned char rms;
	unsigned char fheight[8];
	unsigned char fwidth[6];          // width/4  f0-5
	unsigned char fright[3];          // width/4  f0-2
	unsigned char bw[4];        // Klatt bandwidth BNZ /2, f1,f2,f3
	unsigned char klattp[5];    // AV, FNZ, Tilt, Aspr, Skew
} frame_t2;   //  TESTING


#ifdef deleted
typedef struct {
	short frflags;
	unsigned char length;
	unsigned char rms;
	short ffreq[9];
	unsigned char fheight[9];
	unsigned char fwidth[6];          // width/4
	unsigned char fright[6];          // width/4
	unsigned char fwidth6, fright6;
	unsigned char klattp[N_KLATTP];
} frame_t;

typedef struct {  // 43 bytes
	short frflags;
	unsigned char length;
	unsigned char rms;
	short ffreq[9];
	unsigned char fheight[9];
	unsigned char fwidth[6];          // width/4
	unsigned char fright[6];          // width/4
} frame_t2;   //  the original, without Klatt additions, used for file "phondata" 
#endif



// formant data used by wavegen
typedef struct {
	int freq;     // Hz<<16
	int height;   // height<<15
	int left;     // Hz<<16
	int right;    // Hz<<16
	DOUBLEX freq1; // floating point versions of the above
	DOUBLEX height1;
	DOUBLEX left1;
	DOUBLEX right1;
	DOUBLEX freq_inc;    // increment by this every 64 samples
	DOUBLEX height_inc;
	DOUBLEX left_inc;
	DOUBLEX right_inc;
}  wavegen_peaks_t;

typedef struct {
unsigned char *pitch_env;
int pitch;          // pitch Hz*256
int pitch_ix;       // index into pitch envelope (*256)
int pitch_inc;      // increment to pitch_ix
int pitch_base;     // Hz*256 low, before modified by envelope
int pitch_range;    // Hz*256 range of envelope

unsigned char *mix_wavefile;  // wave file to be added to synthesis
int n_mix_wavefile;       // length in bytes
int mix_wave_scale;         // 0=2 byte samples
int mix_wave_amp;
int mix_wavefile_ix;

int amplitude;
int amplitude_v;
int prev_was_synth;  // previous sound was synthesized (not a played wave or pause)
} WGEN_DATA;


typedef struct {
	double a;
	double b;
	double c;
	double x1;
	double x2;
}  RESONATOR;


typedef struct {
   short length_total;  // not used
   unsigned char  n_frames;
   unsigned char  flags;
   frame_t2  frame[N_SEQ_FRAMES];     // max. frames in a spectrum sequence
} SPECT_SEQ;   // sequence of espeak formant frames

typedef struct {
   short length_total;  // not used
   unsigned char  n_frames;
   unsigned char  flags;
   frame_t  frame[N_SEQ_FRAMES];     // max. frames in a spectrum sequence
} SPECT_SEQK;   // sequence of klatt formants frames


typedef struct {
	short length;
	short frflags;
	frame_t *frame;
} frameref_t;


typedef struct {
	PHONEME_TAB *ph;
	unsigned char env;    // pitch envelope number
	unsigned char stresslevel;
	unsigned char type;
	unsigned char prepause;
	unsigned char amp;
	unsigned char tone_ph;   // tone phoneme to use with this vowel
	unsigned char newword;   // bit 0=start of word, bit 1=end of clause, bit 2=start of sentence
	unsigned char synthflags;
	short length;  // length_mod
	short pitch1;  // pitch, 0-4095 within the Voice's pitch range
	short pitch2;
	unsigned short sourceix;  // ix into the original source text string, only set at the start of a word
} PHONEME_LIST;


typedef struct {
	int name;
	int length;
	char *data;
	char *filename;
} SOUND_ICON;

typedef struct {
	int  name;
	unsigned int  next_phoneme;
	int  mbr_name;
	int  mbr_name2;
	int  percent;         // percentage length of first component
	int  control;
} MBROLA_TAB;

typedef struct {
	int speed_factor1;
	int speed_factor2;
	int speed_factor3;
	int min_sample_len;
	int fast_settings[8];
} SPEED_FACTORS;


// phoneme table
extern PHONEME_TAB *phoneme_tab[N_PHONEME_TAB];

// list of phonemes in a clause
extern int n_phoneme_list;
extern PHONEME_LIST phoneme_list[N_PHONEME_LIST];
extern unsigned int embedded_list[];

extern unsigned char env_fall[128];
extern unsigned char env_rise[128];
extern unsigned char env_frise[128];

#define MAX_PITCH_VALUE  101
extern unsigned char pitch_adjust_tab[MAX_PITCH_VALUE+1];

// queue of commands for wavegen
#define WCMD_KLATT	1
#define WCMD_KLATT2	2
#define WCMD_SPECT	3
#define WCMD_SPECT2	4
#define WCMD_PAUSE	5
#define WCMD_WAVE    6
#define WCMD_WAVE2   7
#define WCMD_AMPLITUDE 8
#define WCMD_PITCH	9
#define WCMD_MARKER	10
#define WCMD_VOICE   11
#define WCMD_EMBEDDED 12


#define N_WCMDQ   160
#define MIN_WCMDQ  22   // need this many free entries before adding new phoneme

extern long wcmdq[N_WCMDQ][4];
extern int wcmdq_head;
extern int wcmdq_tail;

// from Wavegen file
int  WcmdqFree();
void WcmdqStop();
int  WcmdqUsed();
void WcmdqInc();
int  WavegenOpenSound();
int  WavegenCloseSound();
int  WavegenInitSound();
void WavegenInit(int rate, int wavemult_fact);
float polint(float xa[],float ya[],int n,float x);
int WavegenFill(int fill_zeros);
void MarkerEvent(int type, unsigned int char_position, int value, unsigned char *out_ptr);


extern unsigned char *wavefile_data;
extern int samplerate;
extern int samplerate_native;

extern int wavefile_ix;
extern int wavefile_amp;
extern int wavefile_ix2;
extern int wavefile_amp2;
extern int vowel_transition[4];
extern int vowel_transition0, vowel_transition1;

extern int mbrola_delay;
extern char mbrola_name[20];

// from synthdata file
unsigned int LookupSound(PHONEME_TAB *ph1, PHONEME_TAB *ph2, int which, int *match_level, int control);
frameref_t *LookupSpect(PHONEME_TAB *ph1, PHONEME_TAB *prev_ph, PHONEME_TAB *next_ph, int which, int *match_level, int *n_frames, PHONEME_LIST *plist);

unsigned char *LookupEnvelope(int ix);
int LoadPhData();

void SynthesizeInit(void);
int  Generate(PHONEME_LIST *phoneme_list, int *n_ph, int resume);
void MakeWave2(PHONEME_LIST *p, int n_ph);
int  SynthOnTimer(void);
int  SpeakNextClause(FILE *f_text, const void *text_in, int control);
int  SynthStatus(void);
void SetSpeed(int control);
void SetEmbedded(int control, int value);
void SelectPhonemeTable(int number);
int  SelectPhonemeTableName(const char *name);

void Write4Bytes(FILE *f, int value);
int Read4Bytes(FILE *f);
int CompileDictionary(const char *dsource, const char *dict_name, FILE *log, char *err_name,int flags);


extern unsigned char *envelope_data[18];
extern int formant_rate[];         // max rate of change of each formant
extern SPEED_FACTORS speed;

extern long count_samples;
extern int outbuf_size;
extern unsigned char *out_ptr;
extern unsigned char *out_start;
extern unsigned char *out_end;
extern int event_list_ix;
extern espeak_EVENT *event_list;
extern t_espeak_callback* synth_callback;
extern int option_log_frames;
extern const char *version_string;
extern const int version_phdata;

#define N_SOUNDICON_TAB  80   // total entries in soundicon_tab
#define N_SOUNDICON_SLOTS 4    // number of slots reserved for dynamic loading of audio files
extern int n_soundicon_tab;
extern SOUND_ICON soundicon_tab[N_SOUNDICON_TAB];

espeak_ERROR SetVoiceByName(const char *name);
espeak_ERROR SetVoiceByProperties(espeak_VOICE *voice_selector);
espeak_ERROR LoadMbrolaTable(const char *mbrola_voice, const char *phtrans, int srate);
void SetParameter(int parameter, int value, int relative);
void MbrolaTranslate(PHONEME_LIST *plist, int n_phonemes, FILE *f_mbrola);
//int MbrolaSynth(char *p_mbrola);
int DoSample(PHONEME_TAB *ph1, PHONEME_TAB *ph2, int which, int length_mod, int amp);
int DoSpect(PHONEME_TAB *this_ph, PHONEME_TAB *prev_ph, PHONEME_TAB *next_ph,
		int which, PHONEME_LIST *plist, int modulation);
int PauseLength(int pause, int control);
int LookupPhonemeTable(const char *name);

void InitBreath(void);

void KlattInit();
int Wavegen_Klatt2(int length, int modulation, int resume, frame_t *fr1, frame_t *fr2);
