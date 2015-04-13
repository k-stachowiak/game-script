" Vim syntax file
" Language: MOON
" Maintainer: Krzysiek Stachowiak
" Latest Revision: 01 April 2015

if exists("b:current_syntax")
    finish
endif

syn keyword basicKeywords do and or if switch try func bind
syn keyword stdFunctions sqrt floor ceil round
syn keyword stdFunctions push_front push_back cat length at slice
syn keyword stdFunctions print format to_string parse parse_bool parse_char parse_int parse_real
syn keyword literalKeywords true false _

syn match mnComment "#.*$"

syn match mnNumber '\d\+'
syn match mnNumber '[-+]\d\+'
syn match mnNumber '\d\+\.\d+'
syn match mnNumber '[-+]\d\+\.\d+'
syn match mnSymbol '[^\d]\S*' contained
syn region mnString start='"' end='"'
syn region mnChar start='\'' end='\''

syn region mnList start="(" end=")" fold transparent

let b:current_syntax = "moon"

hi def link basicKeywords Keyword
hi def link stdFunctions Function
hi def link mnSpecialFunctions Function
hi def link mnComment Comment
hi def link mnNumber Constant
hi def link mnString String
hi def link mnChar Character
hi def link literalKeywords Constant
hi def link mnList Statement

