#include <speak_lib.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#define sleep(x) Sleep(1000*x)
#endif

int callback(short* wav, int num, espeak_EVENT *pEv)
{
  int cEv = 0;
  while (pEv->type) {
    cEv++;
    pEv++;
	}
  printf("callback, events: %d\n", cEv);
  return 0;
}

main()
{
  int nRate = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 10000, NULL, 0);
  printf("nRate: %d\n", nRate);
  if (nRate < 0)
    return;
  espeak_SetSynthCallback(callback);
  espeak_SetParameter(espeakRATE, 175, 0);
  int rv;
  rv = espeak_Char('c');
  printf("rv: %d\n", rv);
  const char* sText = "tralalalallala";
  rv = espeak_Synth(sText, 100, 0, POS_CHARACTER, 0, 0, 0, 0);
  printf("rv: %d (full: %d, internal: %d)\n",
    rv, EE_BUFFER_FULL, EE_INTERNAL_ERROR);
  sleep(2); // 2 seconds
  puts("slept");
  espeak_Terminate();
}
