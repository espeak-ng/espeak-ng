#  Using eSpeak NG as a library

- [Simple steps](#simple-steps)
- [Error solutions](#error-solutions)
- [Additional info](#additional-info)

## In simple steps

1. Ensure you have satisfied the [building dependencies](building.md)

2. To use eSpeak NG as a C library, create a file called `test-espeak.c` with the following content:


    #include <espeak-ng/speak_lib.h>
    
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    char  *path = NULL;
    void  *user_data;
    unsigned int *identifier;
    
    int
    main(int argc, char *argv[])
    {
            char *voicename = "English";            /* Set voice by its name */
            char *text      = "Hello world!";
            int   buflength = 500,
            int   options   = 0;

            unsigned int position      = 0,
                         position_type = 0,
                         end_position  = 0,
                         flags         = espeakCHARS_AUTO;

            espeak_Initialize(output, buflength, path, options);
            espeak_SetVoiceByName(voicename);

            printf("Saying  '%s'...\n", text);
            espeak_Synth(text,          /* The text to be synthesized */
                         buflength,     /* <description missing>      */
                         position,      /* <description missing>      */
                         position_type, /* <description missing>      */
                         end_position,  /* <description missing>      */
                         flags,         /* <description missing>      */
                         identifier,    /* <description missing>      */
                         user_data);    /* <description missing>      */
            printf("Done\n");
    }

3. Compile the created file with gcc using the command:

    gcc -o test-espeak test-espeak.c -lespeak-ng 

4. Run the created binary with:

    ./test-espeak

## Error solutions

If, during compilation, you get an error similar to `espeak-ng/speak_lib.h: No such file or directory`,
you may try to replace the line:

    #include <espeak-ng/speak_lib.h>

with

    #include "/your/path/to/speak_lib.h"

where `/your/path/to/` is the actual path where the header file is located.

## Additional info

The code presented above initialized the library using just the voice name describing its [language](languages.md).
The code below shows how to set more detailed properties of the voice (e.g. language, variant and gender):

    #include <string.h>
    #include <malloc.h>
    #include <espeak-ng/speak_lib.h>
    
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    char               *path = NULL; /* TODO: path to what?    */
    void               *user_data;   /* TODO: what user data?  */
    unsigned int       *identifier;  /* TODO: what identifier? */
    
    int
    main(int argc, char *argv[])
    {
            espeak_VOICE  voice;
            const char   *text;
            int           buflen;
            int           options;
            unsigned int  position_type;
            unsigned int  position;
            unsigned int  end_position;
            unsigned int  flags;

            text    = "Hello world!";
            buflen  = 500; 
            options = 0;

            position_type = 0;
            position      = 0;
            end_position  = 0;
            flags         = espeakCHARS_AUTO;

            espeak_Initialize(output, buflength, path, options);

            memset(&voice, 0, sizeof(espeak_VOICE)); /* Zero out the voice first */

            const char *langs = "en";                /* Set voice by properties */

            voice.languages = langs;
            voice.name      = "US";
            voice.variant   = 2;
            voice.gender    = 2;

            espeak_SetVoiceByProperties(&voice);

            printf("Saying  '%s'...\n", text);
            espeak_Synth(text,          /* The text to be synthesized */
                         buflen,        /* TODO: describe this arg    */
                         position,      /* TODO: describe this arg    */
                         position_type, /* TODO: describe this arg    */
                         end_position,  /* TODO: describe this arg    */ 
                         flags,         /* TODO: describe this arg    */
                         identifier,    /* TODO: describe this arg    */
                         user_data);    /* TODO: describe this arg    */

            printf("Done\n");
    }

Look for details at [espeak_ng.h](https://github.com/espeak-ng/espeak-ng/blob/master/src/include/espeak-ng/espeak_ng.h) and [speak_lib.h](https://github.com/espeak-ng/espeak-ng/blob/master/src/include/espeak-ng/speak_lib.h) files.