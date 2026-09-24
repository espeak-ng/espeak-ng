" Vim syntax file
" Language: eSpeak NG MBROLA Phoneme Translation Table (phsource/mbrola/*)
" Maintainer: eSpeak-NG Developer

if exists("b:current_syntax")
  finish
endif

" Comments
syntax keyword espeakMbrolaTodo TODO FIXME NOTE contained
syntax match   espeakMbrolaComment "//.*$" contains=espeakMbrolaTodo
syntax match   espeakMbrolaComment "#.*$" contains=espeakMbrolaTodo

" Control Flags (Leading numbers like 0, 8, 9, etc.)
syntax match   espeakMbrolaFlag    "^\s*\zs\d\+\ze\s\+"

" Phonemes and Special Modifiers (j, j:, _, :, etc.)
syntax keyword espeakMbrolaSpecial NULL VWL
syntax match   espeakMbrolaPhoneme "\v\s+[a-zA-Z_][a-zA-Z0-9_:/]*\ze\s+"

" Percentage values for split frames (e.g., 50)
syntax match   espeakMbrolaPercent "\v\s+\d{1,3}\ze\s+"

" Highlight links
hi def link espeakMbrolaComment  Comment
hi def link espeakMbrolaTodo     Todo
hi def link espeakMbrolaFlag     Number
hi def link espeakMbrolaPhoneme  Identifier
hi def link espeakMbrolaPercent  Type
hi def link espeakMbrolaSpecial  Special

let b:current_syntax = "espeakmbrola"
