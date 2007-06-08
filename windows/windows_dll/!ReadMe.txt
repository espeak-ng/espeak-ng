
This is a Virtual C++ project for the Windows dll
version of eSpeak.  This provides the API which is
defined in speak_lib.h, using the 
AUDIO_OUTPUT_SYNCHRONOUS mode only.

This is not the sapi5 version of eSpeak.

Copy the program source files from the Linux "src"
directory into this "src" directory, EXCEPT for:
  speak_lib.h
  speech.h
  StdAfx.h
  stdint.h
Keep the Windows versions of these files.

