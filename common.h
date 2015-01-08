/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/* Common type definitions.
 * ========================
 */

#define VAL_LOC_T ptrdiff_t

#define VAL_SIZE_T uint32_t
#define VAL_SIZE_BYTES sizeof(VAL_SIZE_T)

#define VAL_HEAD_TYPE_T uint32_t
#define VAL_HEAD_SIZE_T VAL_SIZE_T
#define VAL_HEAD_TYPE_BYTES sizeof(VAL_HEAD_TYPE_T)
#define VAL_HEAD_SIZE_BYTES VAL_SIZE_BYTES
#define VAL_HEAD_BYTES (VAL_HEAD_TYPE_BYTES + VAL_HEAD_SIZE_BYTES)

#define VAL_BOOL_T uint8_t
#define VAL_CHAR_T uint8_t
#define VAL_INT_T int64_t
#define VAL_REAL_T double

#define VAL_BOOL_BYTES sizeof(VAL_BOOL_T)
#define VAL_CHAR_BYTES sizeof(VAL_CHAR_T)
#define VAL_INT_BYTES sizeof(VAL_INT_T)
#define VAL_REAL_BYTES sizeof(VAL_REAL_T)

#define VAL_PTR_BYTES sizeof(void*)

/* Portable terminal.
 * ==================
 */

char *my_getline(bool *eof);
char *my_getfile(char *filename);

#endif
