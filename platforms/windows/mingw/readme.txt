These are instructions to built the library on MS Windows, using msys and mingw.
This library is fully functional, contrary to the library described
in windows_dll directory.

To build exe file go to windows_cmd directory, to build sapi dll,
go to windows_sapi directory.

- Use sources from src directory.
- Have a ready to use portaudio v19 library (dll file is sufficient, here
  libportaudio-2.dll filename is assumed in a directory relative to espeak)
- Copy portaudio.h file from your portaudio directory to src directory,
  overwriting the existing one. At the time of writing this is the same
  file as src/portaudio19.h distributed within espeak.

Make command:
  make libespeak.dll PLATFORM_WINDOWS=1 LIB_AUDIO="-L ../../portaudio/lib/.libs -lportaudio-2 -lwinmm"
