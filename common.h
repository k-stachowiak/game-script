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
