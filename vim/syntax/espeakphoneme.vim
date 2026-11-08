" Vim syntax file
" Language: eSpeak NG Phoneme Table
" Latest Revision: 2026

if exists("b:current_syntax")
  finish
endif

" Phoneme Block Region
syn region espeakPhonemeBlock
      \ start="^\s*phoneme\>"
      \ end="^\s*endphoneme\>"
      \ fold
      \ contains=
        \espeakPhonemeKeyword,
        \espeakPhonemeName,
        \espeakInstruction,
        \espeakVowelLine,
        \espeakProperty,
        \espeakConditional,
        \espeakCondition,
        \espeakAttribute,
        \espeakTransition,
        \espeakGroup,
        \espeakNumber,
        \espeakString,
        \espeakComment

" Keywords & Control Flow
syn keyword espeakPhonemeKeyword
  \ phoneme
  \ endphoneme
  \ phonemetable
  \ include
  \ import_phoneme
  \ contained
syn keyword espeakConditional
  \ IF
  \ THEN
  \ ELIF
  \ ELSE
  \ ENDIF
  \ contained

" Phoneme Name (captures token immediately following 'phoneme')
syn match espeakPhonemeName /[@A-Za-z_:]+/ contained

syn match espeakGroup /#[@aeiou]/ contained
syn keyword espeakGroupKeyword starttype endtype contained
syn match espeakVowelLine /^\s*\<vwl\>\s\+starttype\s\+#[@aeiou]\s\+endtype\s\+#[@aeiou]/ contains=espeakProperty,espeakGroupKeyword,espeakGroup contained

" Instructions
syn keyword espeakInstruction length ipa WAV FMT addWav contained
syn keyword espeakInstruction VowelStart VowelEnding Vowelin Vowelout contained
syn keyword espeakInstruction AppendPhoneme InsertPhoneme ChangePhoneme contained
syn keyword espeakInstruction ChangeIfDiminished ChangeIfUnstressed contained
syn keyword espeakInstruction ChangeIfNotStressed ChangeIfStressed contained
syn keyword espeakInstruction IfNextVowelAppend RETURN CALL contained

" Properties & Types (including 'vwl', 'starttype', 'endtype')
syn keyword espeakProperty liquid pause stress virtual vwl contained
syn keyword espeakProperty rhotic unstressed nolink nopause trill contained
syn keyword espeakProperty starttype endtype lengthmod voicingswitch contained

" Conditions & Attributes
syn keyword espeakCondition thisPh prevPh prevPhW prev2PhW nextPh next2Ph nextPhW next2PhW next3PhW nextVowel prevVowel PreVoicing KlattSynth contained
syn keyword espeakAttribute isPause isPause2 isVowel isNotVowel isLiquid isNasal isUStop isVStop isVFricative isPalatal isRhotic isSibilant isVelar isWordStart isWordEnd isFirstVowel isSecondVowel isFinalVowel isAfterStress isVoiced isDiminished isUnstressed isNotStressed isStressed isMaxStress isLong isFlag1 isFlag2 isTranslationGiven contained

" Vowel Transition Parameters
syn keyword espeakTransition len rms f1 f2 f3 brk rate glstop contained

" Groups (e.g., #a, #e, #@, etc.)
syn match espeakGroup /#[@a-zA-Z0-9]\+/ contained

" Comments & Numbers & Strings (Global or contained)
syn match espeakComment "//.*$" contains=espeakTodo
syn match espeakTodo    /\(TODO\|FIXME\|\!\!\!\)/ contained
syn region espeakString start=+"+ end=+"+
syn region espeakPath   start=+<+ end=+>+
syn match espeakNumber  /\<\d+\>/ contained

" Global highlights for statements outside blocks
syn keyword espeakConditional      IF THEN ELIF ELSE ENDIF

" Link Definitions
hi link espeakPhonemeBlock   Function
hi link espeakPhonemeKeyword Type
hi link espeakPhonemeName    Identifier
hi link espeakConditional    Conditional
hi link espeakInstruction    Statement
hi link espeakVowelLine      Statement
hi link espeakProperty       Type
hi link espeakCondition      Statement
hi link espeakAttribute      Identifier
hi link espeakTransition     Special
hi link espeakGroup          Special
hi link espeakComment        Comment
hi link espeakTodo           Todo
hi link espeakString         String
hi link espeakPath           Include
hi link espeakNumber         Number

let b:current_syntax = "espeakphonemes"
