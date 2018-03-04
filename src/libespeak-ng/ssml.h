// stack for language and voice properties
// frame 0 is for the defaults, before any ssml tags.
typedef struct {
        int tag_type;
        int voice_variant_number;
        int voice_gender;
        int voice_age;
        char voice_name[40];
        char language[20];
} SSML_STACK;

#define N_PARAM_STACK  20

#define SSML_SPEAK     1
#define SSML_VOICE     2
#define SSML_PROSODY   3
#define SSML_SAYAS     4
#define SSML_MARK      5
#define SSML_SENTENCE  6
#define SSML_PARAGRAPH 7
#define SSML_PHONEME   8
#define SSML_SUB       9
#define SSML_STYLE    10
#define SSML_AUDIO    11
#define SSML_EMPHASIS 12
#define SSML_BREAK    13
#define SSML_IGNORE_TEXT 14
#define HTML_BREAK    15
#define HTML_NOSPACE  16   // don't insert a space for this element, so it doesn't break a word
#define SSML_CLOSE    0x20 // for a closing tag, OR this with the tag type

int LoadSoundFile2(const char *fname);
int AddNameData(const char *name, int wide);
int ProcessSsmlTag(wchar_t *xml_buf, char *outbuf, int *outix, int n_outbuf, bool self_closing, const char *xmlbase, bool *audio_text, char *current_voice_id, espeak_VOICE *base_voice, char *base_voice_variant_name, bool *ignore_text, bool *clear_skipping_text, int *sayas_mode, int *sayas_start, SSML_STACK *ssml_stack, int *n_ssml_stack, int *n_param_stack, int *speech_parameters);
