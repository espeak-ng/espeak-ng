/*
 * Copyright (C) 2017 Reece H. Dunn
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "speech.h"
#include "phoneme.h"
#include "voice.h"
#include "synthesize.h"
#include "translate.h"

// region espeak_Initialize

static void
test_espeak_terminate_without_initialize()
{
	printf("testing espeak_Terminate without espeak_Initialize\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_initialize()
{
	printf("testing espeak_Initialize\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

// endregion
// region espeak_Synth

static void
test_espeak_synth()
{
	printf("testing espeak_Synth\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_synth_no_voices(const char *path)
{
	printf("testing espeak_Synth in path with no voices\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, path, espeakINITIALIZE_DONT_EXIT) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	int res = espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);
#if USE_ASYNC
	assert(res == EE_OK);
#else
	assert(res == EE_NOT_FOUND);
#endif
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Synchronize() == EE_OK);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

// endregion
// region espeak_ng_Synthesize

static void
test_espeak_ng_synthesize()
{
	printf("testing espeak_ng_Synthesize\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_ng_Synthesize(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == ENS_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_ng_synthesize_no_voices(const char *path)
{
	printf("testing espeak_ng_Synthesize in path with no voices\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, path, espeakINITIALIZE_DONT_EXIT) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	int res = espeak_ng_Synthesize(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);
#if USE_ASYNC
	assert(res == EE_OK);
#else
	assert(res == ENS_VOICE_NOT_FOUND);
#endif
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_ng_Synchronize() == ENS_OK);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

// endregion
// region espeak_SetVoiceByName

static void
test_espeak_set_voice_by_name_null_voice()
{
	printf("testing espeak_SetVoiceByName(NULL)\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_SetVoiceByName("") == EE_NOT_FOUND);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_name_blank_voice()
{
	printf("testing espeak_SetVoiceByName(\"\")\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_SetVoiceByName("") == EE_NOT_FOUND);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_name_valid_voice()
{
	printf("testing espeak_SetVoiceByName(\"de\")\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_SetVoiceByName("de") == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "de") == 0);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "de") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "de") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_name_invalid_voice()
{
	printf("testing espeak_SetVoiceByName(\"zzz\")\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_SetVoiceByName("zzz") == EE_NOT_FOUND);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_name_language_variant_intonation_parameter()
{
	printf("testing espeak_SetVoiceByName(\"!v/Annie\") (language variant; intonation)\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_SetVoiceByName("!v/Annie") == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

// endregion
// region espeak_SetVoiceByProperties

static void
test_espeak_set_voice_by_properties_empty()
{
	printf("testing espeak_SetVoiceByProperties: (none)\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	espeak_VOICE properties;
	memset(&properties, 0, sizeof(properties));

	assert(espeak_SetVoiceByProperties(&properties) == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_properties_blank_language()
{
	printf("testing espeak_SetVoiceByProperties: languages=\"\"\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	espeak_VOICE properties;
	memset(&properties, 0, sizeof(properties));
	properties.languages = "";

	assert(espeak_SetVoiceByProperties(&properties) == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_properties_with_valid_language()
{
	printf("testing espeak_SetVoiceByProperties: languages=\"mk\" (valid)\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	espeak_VOICE properties;
	memset(&properties, 0, sizeof(properties));
	properties.languages = "mk";

	assert(espeak_SetVoiceByProperties(&properties) == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "mk") == 0);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "mk") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "mk") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static void
test_espeak_set_voice_by_properties_with_invalid_language()
{
	printf("testing espeak_SetVoiceByProperties: languages=\"zzz\" (invalid)\n");

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	espeak_VOICE properties;
	memset(&properties, 0, sizeof(properties));
	properties.languages = "zzz";

	assert(espeak_SetVoiceByProperties(&properties) == EE_NOT_FOUND);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
#if !USE_ASYNC
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);
#endif

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);
	assert(strcmp(translator->dictionary_name, "en") == 0);
	assert(p_decoder != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

static int
_test_espeak_ng_phoneme_events_cb(short *samples, int num_samples, espeak_EVENT *events) {
	char *out = events->user_data;
	size_t offset;
	for (espeak_EVENT *e = events; e->type != 0; e++) {
		if (e->type == espeakEVENT_PHONEME) {
			if (out[0] == 0) offset = 0;
			else {
				offset = strlen(out);
				out[offset++] = ' ';
			}
			strncpy(out + offset, e->id.string, sizeof(e->id.string));
		}
	}
	return 0;
}

static void
test_espeak_ng_phoneme_events(int enabled, int ipa) {
	printf("testing espeak_ng_SetPhonemeEvents(enabled=%d, ipa=%d)\n", enabled, ipa);

	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);

	espeak_ng_InitializePath(NULL);
	espeak_ng_ERROR_CONTEXT context = NULL;
	assert(espeak_ng_Initialize(&context) == ENS_OK);
	assert(espeak_ng_InitializeOutput(0, 0, NULL) == ENS_OK);
	espeak_SetSynthCallback(_test_espeak_ng_phoneme_events_cb);
	assert(espeak_ng_SetPhonemeEvents(enabled, ipa) == ENS_OK);

	char phoneme_events[256];
	memset(phoneme_events, 0, sizeof(phoneme_events));
	const char *test = "test";

	assert(espeak_ng_Synthesize(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, phoneme_events) == ENS_OK);
	assert(espeak_ng_Synchronize() == ENS_OK);
	if (enabled) {
		if (ipa) {
			assert(strncmp(phoneme_events, "t ɛ s t  ", sizeof(phoneme_events)) == 0);
		} else {
			assert(strncmp(phoneme_events, "t E s t _: _", sizeof(phoneme_events)) == 0);
		}
	} else {
		assert(phoneme_events[0] == 0);
	}

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
	assert(p_decoder == NULL);
}

// endregion

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter

	char *progdir = strdup(argv[0]);
	char *dir = strrchr(progdir, '/');
	if (dir != NULL) *dir = 0;

	test_espeak_terminate_without_initialize();
	test_espeak_initialize();

	test_espeak_synth();
	test_espeak_synth(); // Check that this does not crash when run a second time.
	test_espeak_synth_no_voices(progdir);
	test_espeak_synth();

	test_espeak_ng_synthesize();
	test_espeak_ng_synthesize(); // Check that this does not crash when run a second time.
	test_espeak_ng_synthesize_no_voices(progdir);
	test_espeak_ng_synthesize();

	test_espeak_set_voice_by_name_null_voice();
	test_espeak_set_voice_by_name_blank_voice();
	test_espeak_set_voice_by_name_valid_voice();
	test_espeak_set_voice_by_name_invalid_voice();
	test_espeak_set_voice_by_name_language_variant_intonation_parameter();

	test_espeak_set_voice_by_properties_empty();
	test_espeak_set_voice_by_properties_blank_language();
	test_espeak_set_voice_by_properties_with_valid_language();
	test_espeak_set_voice_by_properties_with_invalid_language();

	test_espeak_ng_phoneme_events(0, 0);
	test_espeak_ng_phoneme_events(1, 0);
	test_espeak_ng_phoneme_events(1, 1);

	free(progdir);

	return EXIT_SUCCESS;
}
