/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef LOG_H
#define LOG_H

#define LLVL_TRACE 0
#define LLVL_DEBUG 1
#define LLVL_ERROR 2
#define LLVL_NONE 3

#define LOG_LEVEL LLVL_TRACE

#if LOG_LEVEL < LLVL_NONE
#   include <stdio.h>
#endif

#if LOG_LEVEL <= LLVL_TRACE
#   define LOG_TRACE(FORMAT, ...) fprintf(stderr, "[TRACE] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#   define LOG_TRACE_FUNC fprintf(stderr, "[TRACE] %s:%d In function %s\n", __FILE__, __LINE__, __FUNCTION__);
#else
#   define LOG_TRACE(...)
#   define LOG_TRACE_FUNC
#endif

#if LOG_LEVEL <= LLVL_DEBUG
#   define LOG_DEBUG(FORMAT, ...) fprintf(stderr, "[DEBUG] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#   define LOG_DEBUG(...)
#endif

#if LOG_LEVEL <= LLVL_ERROR
#   define LOG_ERROR(FORMAT, ...) fprintf(stderr, "[ERROR] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#   define LOG_ERROR(...)
#endif

#endif
