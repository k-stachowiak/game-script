/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef LTR_PARSE_H
#define LTR_PARSE_H

#include <stdbool.h>

bool lp_parse_bool(char *str, bool *boolean);
bool lp_parse_char(char *str, char *character);
bool lp_parse_real(char *str, double *real);
bool lp_parse_int(char *str, long *integer);
bool lp_parse_str(char *str, char **string);

#endif

