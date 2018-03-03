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

// these tags have no effect if they are self-closing, eg. <voice />
static char ignore_if_self_closing[] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0 };

static MNEM_TAB ssmltags[] = {
	{ "speak",     SSML_SPEAK },
	{ "voice",     SSML_VOICE },
	{ "prosody",   SSML_PROSODY },
	{ "say-as",    SSML_SAYAS },
	{ "mark",      SSML_MARK },
	{ "s",         SSML_SENTENCE },
	{ "p",         SSML_PARAGRAPH },
	{ "phoneme",   SSML_PHONEME },
	{ "sub",       SSML_SUB },
	{ "tts:style", SSML_STYLE },
	{ "audio",     SSML_AUDIO },
	{ "emphasis",  SSML_EMPHASIS },
	{ "break",     SSML_BREAK },
	{ "metadata",  SSML_IGNORE_TEXT },

	{ "br",     HTML_BREAK },
	{ "li",     HTML_BREAK },
	{ "dd",     HTML_BREAK },
	{ "img",    HTML_BREAK },
	{ "td",     HTML_BREAK },
	{ "h1",     SSML_PARAGRAPH },
	{ "h2",     SSML_PARAGRAPH },
	{ "h3",     SSML_PARAGRAPH },
	{ "h4",     SSML_PARAGRAPH },
	{ "hr",     SSML_PARAGRAPH },
	{ "script", SSML_IGNORE_TEXT },
	{ "style",  SSML_IGNORE_TEXT },
	{ "font",   HTML_NOSPACE },
	{ "b",      HTML_NOSPACE },
	{ "i",      HTML_NOSPACE },
	{ "strong", HTML_NOSPACE },
	{ "em",     HTML_NOSPACE },
	{ "code",   HTML_NOSPACE },

	{ NULL, 0 }
};

int attrcmp(const wchar_t *string1, const char *string2);
int attrlookup(const wchar_t *string1, const MNEM_TAB *mtab);
int attrnumber(const wchar_t *pw, int default_value, int type);
int attr_prosody_value(int param_type, const wchar_t *pw, int *value_out);
int attrcopy_utf8(char *buf, const wchar_t *pw, int len);
int GetVoiceAttributes(wchar_t *pw, int tag_type, SSML_STACK *ssml_sp, SSML_STACK *ssml_stack, int n_ssml_stack, char current_voice_id[40], espeak_VOICE *base_voice, char base_voice_variant_name[40]);
const char *VoiceFromStack(SSML_STACK *ssml_stack, int n_ssml_stack, espeak_VOICE *base_voice, char base_voice_variant_name[40]);
