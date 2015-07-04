" Vim syntax file
" Language: MOON
" Maintainer: Krzysiek Stachowiak
" Latest Revision: 01 April 2015

if exists("b:current_syntax")
    finish
endif

syn keyword basicKeywords do and or if switch try func bind ref begin end peek poke succ
syn keyword biFunctions sqrt floor ceil round
syn keyword biFunctions eq lt
syn keyword biFunctions and or xor not
syn keyword biFunctions push_front push_back cat length at slice
syn keyword biFunctions print format to_string parse parse_bool parse_char parse_int parse_real
syn keyword biFunctions rand_ui rand_ur rand_ber rand_exp rand_gauss, rand_distr
syn keyword stdFunctions f_and f_or f_not
syn keyword stdFunctions eq_cmp ne_cmp le_cmp gt_cmp ge_cmp ne gt ge le
syn keyword stdFunctions succ_n distance swap un_ref bin_ref
syn keyword stdFunctions empty front back tail rtail dcons rdcons
syn keyword stdFunctions min min_cmp max max_cmp min_ref min_ref_cmp max_ref max_ref_cmp
syn keyword stdFunctions min_element min_element_cmp
syn keyword stdFunctions all_of any_of none_of
syn keyword stdFunctions find find_if find_if_not
syn keyword stdFunctions lexicographical_compare lexicographical_compare_cmp
syn keyword stdFunctions partition partition_cmp
syn keyword stdFunctions quicksort quicksort_cmp
syn keyword stdFunctions sort sort_cmp
syn keyword stdFunctions point map zip_with foldl foldr zip
syn keyword stdFunctions seq range_int
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

