#ifndef MOON_LOG_H
#define MOON_LOG_H

#define LLVL_TRACE 0
#define LLVL_DEBUG 1
#define LLVL_ERROR 2

#define LOG_LEVEL LLVL_ERROR

#if LOG_LEVEL <= LLVL_TRACE
#       define LOG_TRACE(FORMAT, ...) printf("[TRACE] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_TRACE(...)
#endif

#if LOG_LEVEL <= LLVL_DEBUG
#       define LOG_DEBUG(FORMAT, ...) printf("[DEBUG] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_DEBUG
#endif

#if LOG_LEVEL <= LLVL_ERROR
#       define LOG_ERROR(FORMAT, ...) fprintf(stderr, "[ERROR] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_ERROR
#endif

#endif
