#ifndef ESPEAK_NG_SPLAYER_H
#define ESPEAK_NG_SPLAYER_H

#include "synthesize.h"
#include "voice.h"
#include <speechPlayer.h>

#ifdef __cplusplus
extern "C" {
#endif

	void KlattInitSP(void);
	void KlattResetSP(void);
	void KlattFiniSP(void);
	int Wavegen_KlattSP(WGEN_DATA *wdata, voice_t *wvoice, int length, int resume, frame_t *fr1, frame_t *fr2);

#ifdef __cplusplus
}
#endif

#endif
