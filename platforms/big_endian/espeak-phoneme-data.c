
// 20.02.13  Add samplerate 4-bytes at start of phondata
// 14.09.10  Recognize long and short frames in phondata
// 02.09.10  Fix: Q sections were omitted from the converted phondata
// 13.08.10  jonsd: Added Q lines.  Use Address to set the displacement in phondata file.
// 13.02.10  jonsd: Changed for eSpeak version 1.43


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

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
#define FRFLAG_KLATT           0x01   // this frame includes extra data for Klatt synthesizer
typedef struct {  // 64 bytes
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
	unsigned char spare;        // pad to multiple of 4 bytes
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
} frame_t2;



#define N_SEQ_FRAMES   25

typedef struct {
   short length;
   unsigned char  n_frames;
   unsigned char  sqflags;
   frame_t  frame[N_SEQ_FRAMES];
} SPECT_SEQ;


void swap_phondata  (const char *infile, const char *outfile,
                     const char *manifest);
void swap_phonindex (const char *infile, const char *outfile);
void swap_phontab   (const char *infile, const char *outfile);

void usage (const char *program_name);

int xread;  // prevent compiler warning from fread()


int GetFileLength(const char *filename)
{//====================================
	struct stat statbuf;
	
	if(stat(filename,&statbuf) != 0)
		return(0);
	
	if((statbuf.st_mode & S_IFMT) == S_IFDIR)
		//	if(S_ISDIR(statbuf.st_mode))
		return(-2);  // a directory
	
	return(statbuf.st_size);
}  // end of GetFileLength




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
    printf ("Reading from: %s\n", indir);

    sprintf (f1, "%s/phondata", indir);
    sprintf (f2, "%s/temp_1", outdir);

    printf ("Processing phondata ..\n");
    swap_phondata (f1, f2, manifest);
    if(GetFileLength(f1) != GetFileLength(f2))
    {
      fprintf(stderr, "Error: phondata length is different from the original\n");
      exit(1);
    }
    sprintf (f1, "%s/phondata", outdir);
    rename (f2, f1);

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

    free (f1);
    free (f2);

    printf ("Done.\n");

    return 0;
}  // end of main


void swap_phondata  (const char *infile, const char *outfile,
                const char *manifest)
{//==========================================================
    FILE *in, *mfest, *out;
    int displ;
    int displ_out;
    int errorflag_displ = 0;  // only report the first displ mismatch error
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

    xread = fread(buf_4, 4, 1, in);   // version number
    fwrite(buf_4, 4, 1, out);
    xread = fread(buf_4, 4, 1, in);   // sample rate
    fwrite(buf_4, 4, 1, out);

    while (fgets (line, sizeof(line), mfest))
    {
        if(!isupper(line[0])) continue;

        sscanf(&line[2],"%x",&displ);
        fseek(in, displ, SEEK_SET);
        fflush(out);
        displ_out = ftell(out);
        if((errorflag_displ==0) && (displ != displ_out))
        {
           fprintf(stderr, "Length error at the line before:   %s", line);
           errorflag_displ = 1;
        }

        if (line[0] == 'S') {
            SPECT_SEQ buf_spect;
            size_t frame_start;
            int n;

            xread = fread(&buf_spect, 4, 1, in);
            buf_spect.length = (short) SWAP_USHORT (buf_spect.length);
            fwrite(&buf_spect, 4, 1, out);

            for (n = 0; n < buf_spect.n_frames; n++) {
                int k;

                frame_start = ftell(in);
                xread = fread(&buf_spect.frame[0], sizeof(frame_t), 1, in);

                buf_spect.frame[0].frflags = (short)
                    SWAP_USHORT (buf_spect.frame[0].frflags);

// Changed for eSpeak 1.41
                for (k = 0; k < 7; k++) {
                    buf_spect.frame[0].ffreq[k] = (short)
                        SWAP_USHORT (buf_spect.frame[0].ffreq[k]);
                }

                // is this a long or a short frame?
                if(buf_spect.frame[0].frflags & FRFLAG_KLATT)
                {
                   fwrite(&buf_spect.frame[0], sizeof(frame_t), 1, out);
                   fseek(in, frame_start + sizeof(frame_t), SEEK_SET);
                }
                else
                {
                   fwrite(&buf_spect.frame[0], sizeof(frame_t2), 1, out);
                   fseek(in, frame_start + sizeof(frame_t2), SEEK_SET);
                }
           }
        }
        else if (line[0] == 'W') {
            long pos;
            int length;
            char *wave_data;

            xread = fread (buf_4, 4, 1, in);
            fwrite (buf_4, 4, 1, out);

            length = buf_4[1] * 256 + buf_4[0];

            wave_data = (char *) malloc (length);
            if (wave_data == NULL) {
                fprintf (stderr, "Memory allocation error\n");
                exit (1);
            }

            xread = fread (wave_data, 1, length, in);
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

            xread = fread (env_buf, 1, 128, in);
            fwrite (env_buf, 1, 128, out);
        }
        else if (line[0] == 'Q') {
            unsigned char pb[4];
            unsigned length;
            char *buf;

            xread = fread (pb, 1, 4, in);
            fwrite (pb, 1, 4, out);

            length = (pb[2] << 8) + pb[3]; // size in words
            length *= 4;

            buf = (char *) malloc (length);

            xread = fread (buf, length, 1, in);
            fwrite (buf, length, 1, out);

            free (buf);
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
    unsigned short val;

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

    xread = fread (buf_4, 4, 1, in);  // skip first 4 bytes
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
    char buf_8[8];
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

    xread = fread (buf_8, 4, 1, in);
    fwrite (buf_8, 4, 1, out);
    n_phoneme_tables = buf_8[0];

    for (i = 0; i < n_phoneme_tables; i++) {
        int n_phonemes, j;
        char tab_name[N_PHONEME_TAB_NAME];

        xread = fread (buf_8, 8, 1, in);
        fwrite (buf_8, 8, 1, out);

        n_phonemes = buf_8[0];

        xread = fread (tab_name, N_PHONEME_TAB_NAME, 1, in);
        fwrite (tab_name, N_PHONEME_TAB_NAME, 1, out);

        for (j = 0; j < n_phonemes; j++) {
            PHONEME_TAB table;

            xread = fread (&table, sizeof (PHONEME_TAB), 1, in);

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
