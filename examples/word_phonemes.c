/*
 * Example: word-level phoneme records via
 * espeak_TextToWordPhonemePairsWithTerminator().
 *
 * Copyright (C) 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <espeak-ng/speak_lib.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Build & run
 * -----------
 *
 * 1) Build the espeak-ng library first (from the repository root):
 *
 *      cmake -B build && cmake --build build -j
 *
 * 2) Compile this example against the built library.
 *
 *    macOS (shared build):
 *
 *      gcc -I src/include examples/word_phonemes.c \
 *          build/src/libespeak-ng/libespeak-ng.dylib -o word_phonemes
 *
 *    Linux (shared build):
 *
 *      gcc -I src/include examples/word_phonemes.c \
 *          -L build/src/libespeak-ng -lespeak-ng -o word_phonemes
 *
 *    Windows/MSVC: the same command works if compat/getopt.c is added and the
 *    library is linked; getopt() comes from <unistd.h> (POSIX).
 *
 * 3) Run it.  The first argument is the espeak-ng data directory (required
 *    when espeak-ng is not installed system-wide), and on macOS the loader
 *    must be told where the just-built library lives:
 *
 *      DYLD_LIBRARY_PATH=build/src/libespeak-ng ./word_phonemes -p build/espeak-ng-data
 *      # Linux: LD_LIBRARY_PATH=build/src/libespeak-ng ./word_phonemes -p build/espeak-ng-data
 *
 *    Options:
 *      -p <path>   compiled espeak-ng data directory (e.g. build/espeak-ng-data)
 *      -t <text>   text to translate (default: "Version 2.5 uses .NET. Don't stop.")
 *      -v <voice>  voice name (default: en-us; e.g. en, fr, es, hu)
 *
 *    Runnable examples:
 *
 *      ./word_phonemes -p build/espeak-ng-data
 *      ./word_phonemes -p build/espeak-ng-data -t "Héllo wörld. Comment ça va?" -v fr
 *      ./word_phonemes -p build/espeak-ng-data -v es -t "¿Dónde está la estación?"
 *      ./word_phonemes -p build/espeak-ng-data -v hu -t "nem december 7-e"
 */

int main(int argc, char **argv) {
  const char *path = NULL;
  const char *text = "Version 2.5 uses .NET. Don't stop.";
  const char *voice = "en-us";
  const void *textptr;
  int terminator;
  int c;

  while ((c = getopt(argc, argv, "p:t:v:")) != -1) {
    switch (c) {
    case 'p':
      path = optarg;
      break;
    case 't':
      text = optarg;
      break;
    case 'v':
      voice = optarg;
      break;
    default:
      fprintf(stderr,
              "Usage: %s [-p <espeak-ng-data path>] [-t <text>] [-v <voice>]\n",
              argv[0]);
      return 1;
    }
  }
  textptr = text;

  if (espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, path, 0) < 0) {
    fprintf(stderr, "espeak_Initialize failed\n");
    return 1;
  }
  if (espeak_SetVoiceByName(voice) != EE_OK) {
    fprintf(stderr, "espeak_SetVoiceByName(\"%s\") failed\n", voice);
    return 1;
  }

  /* Each call translates one clause and returns the flat clause phoneme
     string plus one pair per word: the word itself and its phonemes. */
  for (;;) {
    espeak_word_phoneme_pairs *pairs =
        espeak_TextToWordPhonemePairsWithTerminator(
            &textptr, espeakCHARS_UTF8, espeakPHONEMES_IPA, &terminator);
    int i;

    if (pairs == NULL)
      break;
    printf("clause_phonemes: %s\n", pairs->clause_phonemes);
    for (i = 0; i < pairs->size_pairs; i++)
      printf("%d\t%d\t%s\t%s\n", pairs->pairs[i].word_position,
             pairs->pairs[i].word_length, pairs->pairs[i].word,
             pairs->pairs[i].phonemes);
    espeak_FreeWordPhonemePairs(pairs);
    if (textptr == NULL)
      break;
  }

  return 0;
}
