CFLAGS = -Wall -Werror -g -O0 -I.
CC = clang

main: main.c common/sstream.c sexpr/tok.c sexpr/dom.c ast/ast.c ast/ast_fcall.c ast/ast_fdecl.c ast/ast_lit.c ast/ast_ref.c ast/ast_cpd.c

clean:
	rm main

.PHONY: clean
