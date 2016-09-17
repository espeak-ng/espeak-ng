" Vim syntax file
" Language:	eSpeak Dictionary Rules Files
" Filenames:    *_rules
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2016 Sep 16

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif


" Replace region
syn region espeakReplace start="^\.replace"hs=s+9 end="^\."me=e-2,he=e-2 contains=espeakReplaceKeyword,espeakComment nextgroup=espeakReplaceKeyword
syn match espeakReplaceKeyword /^\.replace/ contained 

" Rule region
syn region espeakRule start="^\.group"hs=s+6 end="^\."me=e-2,he=e-2 contains=espeakRuleGroupKeyword,espeakRuleName,espeakRuleLine,espeakKeyword,espeakComment nextgroup=espeaGroupName fold

" One rule line in Rule region
syn region espeakRuleLine start="^[^\.]" end="$" contains=espeak1Cols,espeak2Cols,espeak3Cols,espeak4Cols,espeak5Cols,espeakComment

syn match espeakRuleCond     /^?\!\{-}\d\+/ contained
syn match espeakCharGroup    /^\.L\d\{2}/
syn match espeakCharGroup    /L\d\{2}/ contained
syn match espeakRuleSymbol   /[@#&ABCDHFGKNVXYZ]/ contained
syn match espeakRuleSpecial  /[_\-\/&%+<]/ contained
syn match espeakRulePrePoFix /[SP]\d\+/ contained
syn match espeakRuleNote     /\(\$w_alt\d*\|\$p_alt\d*\|$noprefix\)/ contained
syn match espeakComment "//.*$"  contains=espeakTodo 
syn match espeakComment "//.*$"  contains=espeakTodo contained
syn match espeakTodo /\(TODO\|FIXME\|\!\!\!\)/
"syn match espeakError       /^\s*\S\+/ contains=espeakComment

syn match espeakRuleGroupKeyword /^\.group/ nextgroup=espeakGroupName contained
syn match espeakGroupName /\s*\S\+/ nextgroup=espeakComment contained 
syn match espeak1Cols /^\s*\S\+/ contains=espeakComment contained 
syn match espeak2Cols /^\s*\S\+\s\+\S\+/ contains=espeakRuleCond,espeakRuleSpell,espeakCharGroup,espeakComment contained 
syn match espeak3Cols /^\s*\S\+\s\+\S\+\s\+\S\+/ contains=espeakRuleCond,espeakRulePre,espeakRulePos,espeakRuleSpell,espeakComment contained
syn match espeak4Cols /^\s*\S\+\s\+\S\+\s\+\S\+\s\+\S\+/ contains=espeakRuleCond,espeakRulePre,espeakRulePos,espeakRuleSpell,espeakComment contained
syn match espeak5Cols /^\s*\S\+\s\+\S\+\s\+\S\+\s\+\S\+\s\+\S\+/ contains=espeakRuleCond,espeakRulePre,espeakRulePos,espeakRuleSpell,espeakComment contained
syn match espeakRulePre /\S\+)/ contains=espeakCharGroup,espeakRuleSymbol,espeakRuleSpecial contained
syn match espeakRulePos /\s*(\S\+/ contains=espeakCharGroup,espeakRulePrePoFix,espeakRuleNote,espeakRuleSymbol,espeakRuleSpecial contained

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet

hi link espeakReplace String
hi link espeakRuleName Statement
hi link espeakReplaceKeyword Statement 
hi link espeakRuleGroupKeyword Keyword
hi link espeakRuleSpecial espeakRuleHiglight
hi link espeakRuleNote espeakRuleHiglight
hi link espeakRuleCond espeakRuleHiglight 
hi link espeakRulePrePoFix Keyword 
hi link espeakGroupName Character
hi link espeakCharGroup espeakRuleDarklight
hi link espeakRuleSymbol Special
hi link espeak1Cols String
hi link espeak2Cols String
hi link espeak3Cols String
hi link espeak4Cols String
hi link espeak5Cols String
hi link espeakRulePre Identifier
hi link espeakRulePos Identifier
hi link espeakError Error
hi link espeakKeyword Keyword
hi link espeakComment Comment
hi link espeakTodo Todo
hi espeakRuleHiglight guifg=magenta
hi espeakRuleDarklight guifg=darkmagenta

let b:current_syntax = "espeakrules"
" vim: ts=8
"
