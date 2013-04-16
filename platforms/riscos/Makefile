# Project:   Speak3


# Toolflags:
CCflags = -c -C90 -depend !Depend -IC: -throwback -zM -memaccess -L22-S22-L41
C++flags = -c -depend !Depend -IC: -throwback -zM -memaccess -L22-S22-L41
Linkflags = -rmf -c++ -o $@
ObjAsmflags = -throwback -NoCache -depend !Depend
CMHGflags = 
LibFileflags = -c -o $@
Squeezeflags = -o $@


# Final targets:
@.speakmod:   @.o.speak_riscos @.o.compiledict @.o.dictionary \
        @.o.intonation @.o.readclause @.o.setlengths @.o.synthdata \
        @.o.synthesize @.o.translate  @.o.tr_languages @.o.numbers \
	@.o.synth_mbrola @.o.phonemelist \
        @.o.wavegen @.o.voices @.o.assemb @.o.cmhgfile
        Link $(Linkflags) C:o.stubs C:o.c++lib @.o.speak_riscos @.o.compiledict \
        @.o.dictionary @.o.intonation @.o.readclause @.o.setlengths \
        @.o.synthdata @.o.synthesize @.o.numbers @.o.synth_mbrola \
        @.o.translate @.o.wavegen @.o.tr_languages \
        @.o.voices @.o.phonemelist @.o.assemb @.o.cmhgfile


# User-editable dependencies:

# Static dependencies:
@.o.speak_riscos:   @.c.speak_riscos
        cc $(ccflags) -o @.o.speak_riscos @.c.speak_riscos 
@.o.compiledict:   @.c.compiledict
        cc $(ccflags) -o @.o.compiledict @.c.compiledict 
@.o.dictionary:   @.c.dictionary
        cc $(ccflags) -o @.o.dictionary @.c.dictionary 
@.o.intonation:   @.c.intonation
        cc $(ccflags) -o @.o.intonation @.c.intonation
@.o.numbers:   @.c.numbers
        cc $(ccflags) -o @.o.numbers @.c.numbers
@.o.phonemelist:   @.c.phonemelist
        cc $(ccflags) -o @.o.phonemelist @.c.phonemelist
@.o.readclause:   @.c.readclause
        cc $(ccflags) -o @.o.readclause @.c.readclause
@.o.setlengths:   @.c.setlengths
        cc $(ccflags) -o @.o.setlengths @.c.setlengths 
@.o.synthdata:   @.c.synthdata
        cc $(ccflags) -o @.o.synthdata @.c.synthdata 
@.o.synth_mbrola:   @.c.synth_mbrola
        cc $(ccflags) -o @.o.synth_mbrola @.c.synth_mbrola 
@.o.synthesize:   @.c.synthesize
        cc $(ccflags) -o @.o.synthesize @.c.synthesize 
@.o.translate:   @.c.translate
        cc $(ccflags) -o @.o.translate @.c.translate 
@.o.tr_languages:   @.c.tr_languages
        cc $(ccflags) -o @.o.tr_languages @.c.tr_languages 
@.o.voices:   @.c.voices
        cc $(ccflags) -o @.o.voices @.c.voices 
@.o.wavegen:   @.c.wavegen
        cc $(ccflags) -o @.o.wavegen @.c.wavegen 
@.o.assemb:   @.s.assemb
        objasm $(objasmflags) -from @.s.assemb -to @.o.assemb

# Dynamic dependencies:
o.wavegen:	c.wavegen
o.wavegen:	h.StdAfx
o.wavegen:	h.speak_lib
o.wavegen:	h.speech
o.wavegen:	h.phoneme
o.wavegen:	h.synthesize
o.wavegen:	h.voice
o.wavegen:	h.sintab
o.speak_riscos:	c.speak_riscos
o.speak_riscos:	C:h.kernel
o.speak_riscos:	h.speech
o.speak_riscos:	h.speak_lib
o.speak_riscos:	h.phoneme
o.speak_riscos:	h.synthesize
o.speak_riscos:	h.voice
o.speak_riscos:	h.translate
o.synthdata:	c.synthdata
o.synthdata:	h.StdAfx
o.synthdata:	h.wctype
o.synthdata:	h.speak_lib
o.synthdata:	h.speech
o.synthdata:	h.phoneme
o.synthdata:	h.synthesize
o.synthdata:	h.voice
o.synthdata:	h.translate
o.synthdata:	h.wave
o.synthdata:	C:h.stdint
