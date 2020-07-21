#  Using eSpeak NG as a library

- [Simple steps](#simple-steps)
- [Error solutions](#error-solutions)
- [Additional info](#additional-info)

## Simple steps

1. Ensure you have satisfied [building dependencies](building.md)

2. To use eSpeak NG as a C library, create file `test-espeak.c` with following content:


        #include <espeak-ng/speak_lib.h>
        
        espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
        char *path = NULL;
        void* user_data;
        unsigned int *identifier;
        
        int main(int argc, char* argv[]) {
          char voicename[] = {"English"}; // Set voice by its name
          char text[] = {"Hello world!"};
          int buflength = 500, options = 0;
          unsigned int position = 0, position_type = 0, end_position = 0, flags = espeakCHARS_AUTO;
          espeak_Initialize(output, buflength, path, options);
          espeak_SetVoiceByName(voicename);
          printf("Saying  '%s'...\n", text);
          espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
          printf("Done\n");
          return 0;
        }

3. Compile created file with command:

        gcc test-espeak.c -lespeak-ng -o test-espeak

4. Run created binary with the command:

        ./test-espeak

## Error solutions

If, during compilation, you get error like `espeak-ng/speak_lib.h: No such file or directory`,
you may try to replace line:

    #include <espeak-ng/speak_lib.h>

with

    #include "/your/path/to/speak_lib.h"

where `/your/path/to/` is actual path, where header file is located.

## Additional info

Code above set library using just voice name with related [language](languages.md) to it.
Code below shows, how to set more detailed properties of the voice (e.g. language, variant and gender):

    #include <string.h>
    #include <malloc.h>
    #include <espeak-ng/speak_lib.h>
    
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    char *path = NULL;
    void* user_data;
    unsigned int *identifier;
    
    int main(int argc, char* argv[] ) {
      char text[] = {"Hello world!"};
      int buflength = 500, options = 0;
      unsigned int position = 0, position_type = 0, end_position = 0, flags = espeakCHARS_AUTO;
      espeak_Initialize(output, buflength, path, options );
      espeak_VOICE voice;
      memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
      const char *langNativeString = "en"; // Set voice by properties
      voice.languages = langNativeString;
      voice.name = "US";
      voice.variant = 2;
      voice.gender = 2;
      espeak_SetVoiceByProperties(&voice);
      printf("Saying  '%s'...\n", text);
      espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
      printf("Done\n");
      return 0;
    }

Look for details at [espeak_ng.h](https://github.com/espeak-ng/espeak-ng/blob/master/src/include/espeak-ng/espeak_ng.h) and [speak_lib.h](https://github.com/espeak-ng/espeak-ng/blob/master/src/include/espeak-ng/speak_lib.h) files.


