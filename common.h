/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/* Common type definitions/
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

/* Memory management.
 * ==================
 */

void *malloc_or_die(size_t size);
void *calloc_or_die(size_t count, size_t size);
void *realloc_or_die(void *old, size_t size);

/* Algorighms.
 * ===========
 */

#define LIST_APPEND(MLA_ELEMENT, MLA_LIST, MLA_END) \
    do { \
        if (!(*(MLA_END))) { \
            *(MLA_LIST) = *(MLA_END) = (MLA_ELEMENT); \
        } else { \
            (*(MLA_END))->next = (MLA_ELEMENT); \
            *(MLA_END) = (MLA_ELEMENT); \
        } \
    } while(0)

#define ARRAY_FREE(MACRO_ARRAY) \
    do { \
        free((MACRO_ARRAY).data); \
        (MACRO_ARRAY).data = NULL; \
        (MACRO_ARRAY).size = 0; \
        (MACRO_ARRAY).cap = 0; \
    } while(0)

#define ARRAY_COPY(dst, src) \
    do { \
        int size = src.cap * sizeof(*dst.data); \
		dst.data = malloc_or_die(size); \
        memcpy(dst.data, src.data, size); \
        dst.size = src.size; \
        dst.cap = src.cap; \
    } while (0)

#define ARRAY_FIND(MACRO_ARRAY, MACRO_ELEMENT, MACRO_RESULT) \
    do { \
        unsigned i; \
        for (i = 0; i < (MACRO_ARRAY).size; ++i) { \
            if ((MACRO_ARRAY).data[i] == (MACRO_ELEMENT)) { \
                MACRO_RESULT = (MACRO_ELEMENT); \
                break; \
            } \
        } \
    } while(0)

#define ARRAY_APPEND(MACRO_ARRAY, MACRO_ELEMENT) \
    do { \
        if ((MACRO_ARRAY).cap == 0) { \
			(MACRO_ARRAY).data = malloc_or_die(sizeof(*((MACRO_ARRAY).data))); \
            (MACRO_ARRAY).cap = 1; \
        } else if ((MACRO_ARRAY).cap == (MACRO_ARRAY).size) { \
            (MACRO_ARRAY).cap *= 2; \
			(MACRO_ARRAY).data = realloc_or_die(\
                    (MACRO_ARRAY).data,\
                    (MACRO_ARRAY).cap * sizeof(*((MACRO_ARRAY).data))); \
        } \
        (MACRO_ARRAY).data[(MACRO_ARRAY).size++] = (MACRO_ELEMENT); \
    } while (0)

#define ARRAY_REMOVE(MACRO_ARRAY, MACRO_INDEX) \
    do { \
        (MACRO_ARRAY).data[(MACRO_INDEX)] = (MACRO_ARRAY).data[(MACRO_ARRAY).size - 1]; \
        (MACRO_ARRAY).size -= 1; \
    } while(0)

#define ARRAY_POP(MACRO_ARRAY) \
    do { \
		ARRAY_REMOVE((MACRO_ARRAY), (MACRO_ARRAY).size - 1); \
	} while(0)

/* Cross-module data types.
 * ========================
 */

enum SourceLocationType {
    SRC_LOC_NONE,
	SRC_LOC_REGULAR,
	SRC_LOC_BIF,
	SRC_LOC_FUNC_CONTAINED
};

struct SourceLocation {
	enum SourceLocationType type;
    int line;
    int column;
};

struct SourceIter {
    char *first;
    char *current;
    char *last;
    struct SourceLocation loc;
};

void si_init(struct SourceIter *si, char *first, char *last);
void si_adv(struct SourceIter *si);
void si_advn(struct SourceIter *si, int n);
void si_back(struct SourceIter *si);
bool si_eq(struct SourceIter *lhs, struct SourceIter *rhs);

/* Logging facilities.
 * ===================
 */

#define LLVL_TRACE 0
#define LLVL_DEBUG 1
#define LLVL_ERROR 2

#define LOG_LEVEL LLVL_ERROR

#if LOG_LEVEL <= LLVL_TRACE
#   define LOG_TRACE(FORMAT, ...) printf("[TRACE] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#   define LOG_TRACE_FUNC printf("[TRACE] %s:%d In function %s\n", __FILE__, __LINE__, __FUNCTION__);
#else
#   define LOG_TRACE(...)
#   define LOG_TRACE_FUNC
#endif

#if LOG_LEVEL <= LLVL_DEBUG
#   define LOG_DEBUG(FORMAT, ...) printf("[DEBUG] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#   define LOG_DEBUG
#endif

#if LOG_LEVEL <= LLVL_ERROR
#   define LOG_ERROR(FORMAT, ...) fprintf(stderr, "[ERROR] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#   define LOG_ERROR
#endif

/* Portable terminal.
 * ==================
 */

char *my_getline(bool *eof);
char *my_getfile(char *filename);

#endif
