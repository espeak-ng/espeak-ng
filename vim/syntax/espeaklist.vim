" Vim syntax file
" Language:	eSpeak Dictionary List Files
" Filenames:    *_list, *_listx, *_extra
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2016 Jun 19

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn region	espeaklistLineComment			start="^//" end="$" keepend contains=espeaklistComment
syn match	espeaklistLinePhraseEntry 		"^\(\(//\)\@!\)(.*$" contains=espeaklistEntryPhrase
syn match	espeaklistLineEntry 			"^\(\(//\)\@!\)[^?(].*$" contains=espeaklistEntryWord
syn match	espeaklistLineConditionalEntry		"^[?!].*$" contains=espeaklistEntryConditional

syn region	espeaklistComment contained		start="//" end="$" keepend contains=@Spell

syn region	espeaklistFlag contained		start="\$" end="[ \t\r\n]" skipwhite nextgroup=espeaklistFlag,espeaklistComment

syn region	espeaklistEntryConditional contained	start="^[?!]" end="[ \t]" skipwhite nextgroup=espeaklistEntryPhrase,espeaklistEntryWord
syn region	espeaklistEntryPhrase contained		start="^(" end=")" skipwhite nextgroup=espeaklistEntryPronunciation
syn match	espeaklistEntryWord contained		"[^ \t\r\n]\+" skipwhite nextgroup=espeaklistEntryPronunciation,espeaklistFlag
syn match	espeaklistEntryPronunciation contained	"[^ \t\r\n/$!][^ \t\r\n]*" skipwhite nextgroup=espeaklistFlag,espeaklistComment

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet

hi def link espeaklistComment			Comment
hi def link espeaklistFlag			PreProc
hi def link espeaklistEntryConditional		PreProc
hi def link espeaklistEntryPhrase		espeaklistEntry
hi def link espeaklistEntryWord			espeaklistEntry
hi def link espeaklistEntryPronunciation	None
hi def link espeaklistEntry			Identifier

let b:current_syntax = "espeaklist"
" vim: ts=8
