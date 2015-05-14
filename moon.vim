" Vim syntax file
" Language: MOON
" Maintainer: Krzysiek Stachowiak
" Latest Revision: 01 April 2015

if exists("b:current_syntax")
    finish
endif

syn keyword basicKeywords do and or if switch try func bind
syn keyword biFunctions sqrt floor ceil round
syn keyword biFunctions eq lt
syn keyword biFunctions and or xor not
syn keyword biFunctions push_front push_back cat length at slice
syn keyword biFunctions print format to_string parse parse_bool parse_char parse_int parse_real
syn keyword biFunctions rand_ui rand_ur rand_ber rand_exp rand_gauss, rand_distr
syn keyword stdFunctions f_and f_or
syn keyword stdFunctions ne gt ge le
syn keyword stdFunctions empty car cdr front back cons dcons
syn keyword stdFunctions min min_cmp max max_cmp
syn keyword stdFunctions min_element min_element_cmp
syn keyword stdFunctions merge merge_cmp merge_sort merge_sort_cmp
syn keyword stdFunctions sort sort_cmp
syn keyword stdFunctions point map zip_with foldl foldr
syn keyword stdFunctions printf
syn keyword stdFunctions test
syn keyword literalKeywords unit true false _

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
hi def link biFunctions Function
hi def link stdFunctions Identifier
hi def link mnSpecialFunctions Function
hi def link mnComment Comment
hi def link mnNumber Constant
hi def link mnString String
hi def link mnChar Character
hi def link literalKeywords Constant
hi def link mnList Statement

