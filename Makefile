CFLAGS = -Wall -Werror -g -O0 -I.
CC = clang

main:   ./ast/ast_bind.c \
        ./ast/ast_cpd.c \
        ./ast/ast_fcall.c \
        ./ast/ast_lit.c \
        ./ast/ast_ref.c \
        ./ast/ast_fdecl.c \
        ./ast/ast_unit.c \
        ./ast/ast.c \
        ./sexpr/dom.c \
        ./sexpr/tok.c \
        ./main.c \
        ./itpr/itpr_eval.c \
        ./itpr/itpr_scope.c \
        ./common/sstream.c

clean:
	rm main

.PHONY: clean
