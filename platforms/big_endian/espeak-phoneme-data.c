





// 13.02.10  JSD: Changed for eSpeak version 1.43


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#define IS_BIG_ENDIAN 1
#else
#define IS_BIG_ENDIAN 0
#endif


#if IS_BIG_ENDIAN

#  define SWAP_USHORT(val)    ((unsigned short) ( \
     (unsigned short) ((unsigned short) (val) >> 8) |  \
     (unsigned short) ((unsigned short) (val) << 8)))

#  define SWAP_UINT(val)    ((unsigned int) ( \
     (((unsigned int) (val) & (unsigned int) 0x000000ffU) << 24) | \
     (((unsigned int) (val) & (unsigned int) 0x0000ff00U) <<  8) | \
     (((unsigned int) (val) & (unsigned int) 0x00ff0000U) >>  8) | \
     (((unsigned int) (val) & (unsigned int) 0xff000000U) >> 24)))

#else

#  define SWAP_USHORT(val) (val)
#  define SWAP_UINT(val) (val)

#endif



#define N_PHONEME_TAB_NAME  32

// This is a new format for eSpeak 1.43
typedef struct {
	unsigned int  mnemonic;      // 1st char is in the l.s.byte
	unsigned int  phflags;       // bits 16-19 place of articulation
	unsigned short program;
	unsigned char  code;         // the phoneme number
	unsigned char  type;         // phVOWEL, phPAUSE, phSTOP etc
	unsigned char  start_type;
	unsigned char  end_type;
	unsigned char  std_length;   // for vowels, in mS/2;  for phSTRESS, the stress/tone type
	unsigned char  length_mod;   // a length_mod group number, used to access length_mod_tab
	
} PHONEME_TAB;


// This is a new format for eSpeak 1.41
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


#ifdef deleted
// This is the previous format for versions before 1.41
typedef struct {
    short frflags;
    unsigned char length;
    unsigned char rms;
    short ffreq[9];
    unsigned char fheight[9];
    unsigned char fwidth[6];
    unsigned char fright[6];
} frame_t;
#endif

#define N_SEQ_FRAMES   25

typedef struct {
   short length;
   unsigned char  n_frames;
   unsigned char  flags;
   frame_t  frame[N_SEQ_FRAMES];
} SPECT_SEQ;


void swap_phondata  (const char *infile, const char *outfile,
                     const char *manifest);
void swap_phonindex (const char *infile, const char *outfile);
void swap_phontab   (const char *infile, const char *outfile);

void usage (const char *program_name);



int main (int argc, char *argv[])
{//==============================
    const char *indir = "/usr/share/espeak-data";
    const char *outdir = ".";
    const char *manifest = "phondata-manifest";
    char *f1, *f2;

    if (argc > 4)
        usage (argv[0]);

    if (argc > 1) {
        if (strcmp (argv[1], "-h") == 0 ||
            strcmp (argv[1], "--help") == 0)
            usage (argv[0]);

        indir = argv[1];
    }

    if (argc > 2)
        outdir = argv[2];

    if (argc > 3)
        manifest = argv[3];

    f1 = (char *) malloc (strlen (indir) + 20);
    if (f1 == NULL) {
        fprintf (stderr, "Unable to allocate memory\n");
        exit (1);
    }
    f2 = (char *) malloc (strlen (outdir) + 20);
    if (f2 == NULL) {
        fprintf (stderr, "Unable to allocate memory\n");
        exit (1);
    }

#if IS_BIG_ENDIAN
    printf ("Host seems to be big-endian ..\n");
#else
    printf ("Host seems to be little-endian ..\n");
#endif

    sprintf (f1, "%s/phontab", indir);
    sprintf (f2, "%s/temp_1", outdir);

    printf ("Processing phontab ..\n");
    swap_phontab (f1, f2);
    sprintf (f1, "%s/phontab", outdir);
    rename (f2, f1);

    sprintf (f1, "%s/phonindex", indir);
    sprintf (f2, "%s/temp_1", outdir);

    printf ("Processing phonindex ..\n");
    swap_phonindex (f1, f2);
    sprintf (f1, "%s/phonindex", outdir);
    rename (f2, f1);

    sprintf (f1, "%s/phondata", indir);
    sprintf (f2, "%s/temp_1", outdir);

    printf ("Processing phondata ..\n");
    swap_phondata (f1, f2, manifest);
    sprintf (f1, "%s/phondata", outdir);
    rename (f2, f1);

    free (f1);
    free (f2);

    printf ("Done.\n");

    return 0;
}  // end of main


void swap_phondata  (const char *infile, const char *outfile,
                const char *manifest)
{//==========================================================
    FILE *in, *mfest, *out;
    char line[1024];
    unsigned char buf_4[4];

    in = fopen (infile, "rb");
    if (in == NULL) {
        fprintf (stderr, "Unable to read from file %s\n", infile);
        exit (1);
    }

    mfest = fopen (manifest, "rb");
    if (mfest == NULL) {
        fprintf (stderr, "Unable to read from file %s\n", manifest);
        exit (1);
    }

    out = fopen (outfile, "wb");
    if (out == NULL) {
        fprintf (stderr, "Unable to open file %s for writing\n", outfile);
        exit (1);
    }

    fread (buf_4, 4, 1, in);
    fwrite (buf_4, 4, 1, out);

    while (fgets (line, 1024, mfest)) {
        if (line[0] == 'S') {
            SPECT_SEQ buf_spect;
            size_t ix;
            int n;

            fread (&buf_spect.length, 2, 1, in);
            fread (&buf_spect.n_frames, 1, 1, in);
            fseek (in, -3, SEEK_CUR);

            ix = (char *)(&buf_spect.frame[buf_spect.n_frames]) -
                (char *)(&buf_spect);
            ix = (ix+3) & 0xfffc;

            fread (&buf_spect, ix, 1, in);

            buf_spect.length = (short) SWAP_USHORT (buf_spect.length);
            for (n = 0; n < buf_spect.n_frames; n++) {
                int k;

                buf_spect.frame[n].frflags = (short)
                    SWAP_USHORT (buf_spect.frame[n].frflags);

// Changed for eSpeak 1.41
                for (k = 0; k < 7; k++) {
                    buf_spect.frame[n].ffreq[k] = (short)
                        SWAP_USHORT (buf_spect.frame[n].ffreq[k]);
                }
            }

            fwrite (&buf_spect, ix, 1, out);
        }
        else if (line[0] == 'W') {
            long pos;
            int length;
            char *wave_data;

            fread (buf_4, 4, 1, in);
            fwrite (buf_4, 4, 1, out);

            length = buf_4[1] * 256 + buf_4[0];

            wave_data = (char *) malloc (length);
            if (wave_data == NULL) {
                fprintf (stderr, "Memory allocation error\n");
                exit (1);
            }

            fread (wave_data, 1, length, in);
            fwrite (wave_data, 1, length, out);

            pos = ftell (in);
            while((pos & 3) != 0) {
                fgetc (in);
                pos++;
            }

            pos = ftell (out);
            while((pos & 3) != 0) {
                fputc (0, out);
                pos++;
            }

            free (wave_data);
        }
        else if (line[0] == 'E') {
            char env_buf[128];

            fread (env_buf, 1, 128, in);
            fwrite (env_buf, 1, 128, out);
        }
    }

    fclose (in);
    fclose (out);
    fclose (mfest);
}  // end of swap_phondata


void swap_phonindex (const char *infile, const char *outfile)
{//==========================================================
    FILE *in, *out;
    char buf_4[4];
    unsigned int val;

    in = fopen (infile, "rb");
    if (in == NULL) {
        fprintf (stderr, "Unable to read from file %s\n", infile);
        exit (1);
    }

    out = fopen (outfile, "wb");
    if (out == NULL) {
        fprintf (stderr, "Unable to open file %s for writing\n", outfile);
        exit (1);
    }

    fread (buf_4, 4, 1, in);  // skip first 4 bytes
    fwrite(buf_4, 4, 1, out);

    while (! feof (in)) {
        size_t n;

        n = fread (&val, 2, 1, in);
        if (n != 1)
            break;

        val = SWAP_USHORT (val);
        fwrite (&val, 2, 1, out);
    }

    fclose (in);
    fclose (out);
}  // end of swap_phonindex


void swap_phontab (const char *infile, const char *outfile)
{//========================================================
    FILE *in, *out;
    char buf_4[4];
    int i, n_phoneme_tables;

    in = fopen (infile, "rb");
    if (in == NULL) {
        fprintf (stderr, "Unable to read from file %s\n", infile);
        exit (1);
    }

    out = fopen (outfile, "wb");
    if (out == NULL) {
        fprintf (stderr, "Unable to open file %s for writing\n", outfile);
        exit (1);
    }

    fread (buf_4, 4, 1, in);
    fwrite (buf_4, 4, 1, out);
    n_phoneme_tables = buf_4[0];

    for (i = 0; i < n_phoneme_tables; i++) {
        int n_phonemes, j;
        char tab_name[N_PHONEME_TAB_NAME];

        fread (buf_4, 4, 1, in);
        fwrite (buf_4, 4, 1, out);

        n_phonemes = buf_4[0];

        fread (tab_name, N_PHONEME_TAB_NAME, 1, in);
        fwrite (tab_name, N_PHONEME_TAB_NAME, 1, out);

        for (j = 0; j < n_phonemes; j++) {
            PHONEME_TAB table;

            fread (&table, sizeof (PHONEME_TAB), 1, in);

            table.mnemonic = SWAP_UINT (table.mnemonic);
            table.phflags = SWAP_UINT (table.phflags);
            table.program = SWAP_USHORT (table.program);

            fwrite (&table, sizeof (PHONEME_TAB), 1, out);
        }
    }

    fclose (in);
    fclose (out);
}  // end of swap_phontab


void
usage (const char *program_name)
{
    fprintf (stderr,
"This program copies the phontab, phonindex and phondata files from a given\n"
"directory, swapping values to big-endian form if necessary.\n\n"
"Usage:\n"
"  %s [INPUT_DIR] [OUTPUT_DIR] [MANIFEST_FILE]\n\n"
"By default, the MANIFEST_FILE used is a file called 'phondata-manifest' in\n"
"the current directory. The default INPUT_DIR is /usr/share/espeak-data and\n"
"OUTPUT_DIR is the current directory.\n", program_name);
    exit (1);
}
