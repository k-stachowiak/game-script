/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "memory.h"
#include "ltr_parse.h"

#define TRUE_CONSTANT "true"
#define FALSE_CONSTANT "false"
#define CHAR_DELIMITER '\''
#define STR_DELIMITER '"'
#define ESCAPE '\\'

static char *find(char *current, char *last, char value)
{
    while (current != last) {
        if (*current == value) {
            return current;
        }
        ++current;
    }
    return current;
}

static bool all_of(char *current, char *last, int (*f)(int))
{
    while (current != last) {
        if (!f(*current)) {
            return false;
        }
        ++current;
    }
    return true;
}

static char *unescape_string(char *in)
{
    int in_len = strlen(in);
    char *out = mem_calloc(in_len + 1, 1);
    char *write = out;
    while (*in) {

        if (*in != ESCAPE) {
            *write++ = *in++;
            continue;
        }

        switch (*++in) {
            case 'a':
                *write++ = '\a';
                break;
            case 'b':
                *write++ = '\b';
                break;
            case 't':
                *write++ = '\t';
                break;
            case 'n':
                *write++ = '\n';
                break;
            case 'f':
                *write++ = '\f';
                break;
            case 'r':
                *write++ = '\r';
                break;
            case 'v':
                *write++ = '\v';
                break;
            case '\\':
                *write++ = '\\';
                break;
            case '"':
                *write++ = '"';
                break;
            case '\0':
                *write++ = '\0';
                break;
            default:
                mem_free(out);
                return NULL;
        }
        ++in;
    }

    return mem_realloc(out, write - out + 1);
}

bool lp_parse_bool(char *str, bool *boolean)
{
    if (strcmp(str, TRUE_CONSTANT) == 0) {
        *boolean = true;
        return true;
    }

    if (strcmp(str, FALSE_CONSTANT) == 0) {
        *boolean = false;
        return true;
    }

    return false;
}

bool lp_parse_char(char *str, char *character)
{
    int len = strlen(str);

    if (str[0] != CHAR_DELIMITER || str[len - 1] != CHAR_DELIMITER) {
        return false;
    }

    if (len == 3) {
        *character = str[1];
        return true;

    } else if (len == 4 && str[1] == ESCAPE) {
        switch(str[2]) {
         case 'a':
            *character = '\a';
            return true;

         case 'b':
            *character = '\b';
            return true;

         case 't':
            *character = '\t';
            return true;

         case 'n':
            *character = '\n';
            return true;

         case 'f':
            *character = '\f';
            return true;

         case 'r':
            *character = '\r';
            return true;

         case 'v':
            *character = '\v';
            return true;

         case '\\':
            *character = '\\';
            return true;

         case '\'':
            *character = '\'';
            return true;

         case '0':
            *character = '\0';
            return true;

         default:
            return false;
        }
    } else {
        return false;
    }
}

bool lp_parse_real(char *str, double *real)
{
    char *first, *last, *period;
    int len = strlen(str);

    first = str;
    last = first + len;
    period = find(first, last, '.');

    if (str[0] == '-' || str[0] == '+') {
        ++first;
    }

    if (period == last ||
        !all_of(first, period, isdigit) ||
        !all_of(period + 1, last, isdigit)) {
        return false;
    }

    *real = atof(str);
    return true;
}

bool lp_parse_int(char *str, long *integer)
{
    int len = strlen(str);

    if (len > 0) {
        char *first = str, *last = str + len;
        if (str[0] == '-' || str[0] == '+') {
            ++first;
        }
        if (all_of(first, last, isdigit)) {
            *integer = atol(str);
            return true;
        }
    }

    return false;
}

bool lp_parse_str(char *str, char **string)
{
    int len = strlen(str);
    if (str[0] == STR_DELIMITER && str[len - 1] == STR_DELIMITER && len >= 2) {
        *string = unescape_string(str);
        return *string;
    } else {
        return false;
    }
}

