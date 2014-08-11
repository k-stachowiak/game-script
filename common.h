/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>

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
        dst.data = malloc(size); \
        memcpy(dst.data, src.data, size); \
        dst.size = src.size; \
        dst.cap = src.cap; \
    } while (0)

#define ARRAY_FIND(MACRO_ARRAY, MACRO_ELEMENT, MACRO_RESULT) \
    do { \
        int i; \
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
            (MACRO_ARRAY).data = malloc(sizeof(*((MACRO_ARRAY).data))); \
            if (!(MACRO_ARRAY).data) { \
                fprintf(stderr, "Allocation failure.\n"); \
                exit(1); \
            } \
            (MACRO_ARRAY).cap = 1; \
        } else if ((MACRO_ARRAY).cap == (MACRO_ARRAY).size) { \
            (MACRO_ARRAY).cap *= 2; \
            (MACRO_ARRAY).data = realloc(\
                    (MACRO_ARRAY).data,\
                    (MACRO_ARRAY).cap * sizeof(*((MACRO_ARRAY).data))); \
            if (!(MACRO_ARRAY).data) { \
                fprintf(stderr, "Allocation failure.\n"); \
                exit(1); \
            } \
        } \
        (MACRO_ARRAY).data[(MACRO_ARRAY).size++] = (MACRO_ELEMENT); \
    } while (0)

#define ARRAY_REMOVE(MACRO_ARRAY, MACRO_INDEX) \
    do { \
        (MACRO_ARRAY).data[(MACRO_INDEX)] = (MACRO_ARRAY).data[(MACRO_ARRAY).size - 1]; \
        (MACRO_ARRAY).size -= 1; \
    } while(0)

/* Cross-module data types.
 * ========================
 */

struct Location {
    int line;
    int column;
};

struct SourceIter {
    char *first;
    char *current;
    char *last;
    struct Location loc;
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

#endif
