#!/usr/bin/env python3
"""Verifies that the custom espeak_TextToPhonemesWithTerminator function works."""
import ctypes

EE_OK = 0
AUDIO_OUTPUT_SYNCHRONOUS = 0x02
espeakPHONEMES_IPA = 0x02
espeakCHARS_AUTO = 0

CLAUSE_INTONATION_FULL_STOP = 0x00000000
CLAUSE_INTONATION_COMMA = 0x00001000
CLAUSE_INTONATION_QUESTION = 0x00002000
CLAUSE_INTONATION_EXCLAMATION = 0x00003000

CLAUSE_TYPE_SENTENCE = 0x00080000

EXPECTED_PHONEMES = """ðˈɪs, ˌaɪˌɛsˈeɪ tˈɛst!
ˌoʊkˈeɪ?
"""

def main() -> None:
    """Verify espeak_TextToPhonemesWithTerminator function"""
    lib = ctypes.cdll.LoadLibrary("libespeak-ng.so")
    ret = lib.espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, None, 0)
    assert ret > 0, ret

    # Set voice
    voice = "en-us"
    voice_bytes = voice.encode("utf-8")
    ret = lib.espeak_SetVoiceByName(voice_bytes)
    assert ret == EE_OK, ret

    # Will fail if custom function is missing
    lib.espeak_TextToPhonemesWithTerminator.restype = ctypes.c_char_p

    # Should split into 3 sentences, highlighting each punctuation type.
    text = "this, I.S. a test! ok?"
    text_bytes = text.encode("utf-8")
    text_pointer = ctypes.c_char_p(text_bytes)

    phoneme_flags = espeakPHONEMES_IPA
    text_flags = espeakCHARS_AUTO

    phonemes = ""
    while text_pointer:
        terminator = ctypes.c_int(0)
        clause_phonemes = lib.espeak_TextToPhonemesWithTerminator(
            ctypes.pointer(text_pointer),
            text_flags,
            phoneme_flags,
            ctypes.pointer(terminator),
        )
        if isinstance(clause_phonemes, bytes):
            phonemes += clause_phonemes.decode()

        # Check for punctuation.
        # The testing order here is critical.
        if (terminator.value & CLAUSE_INTONATION_EXCLAMATION) == CLAUSE_INTONATION_EXCLAMATION:
            phonemes += "!"
        elif (terminator.value & CLAUSE_INTONATION_QUESTION) == CLAUSE_INTONATION_QUESTION:
            phonemes += "?"
        elif (terminator.value & CLAUSE_INTONATION_COMMA) == CLAUSE_INTONATION_COMMA:
            phonemes += ","
        elif (terminator.value & CLAUSE_INTONATION_FULL_STOP) == CLAUSE_INTONATION_FULL_STOP:
            phonemes += "."

        # Check for end of sentence
        if (terminator.value & CLAUSE_TYPE_SENTENCE) == CLAUSE_TYPE_SENTENCE:
            phonemes += "\n"
        else:
            phonemes += " "

    assert phonemes == EXPECTED_PHONEMES, f"Expected: {EXPECTED_PHONEMES}\n Got: {phonemes}"

    print("It works!")


if __name__ == "__main__":
    main()
