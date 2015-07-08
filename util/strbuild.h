/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef STRBUILD_H
#define STRBUILD_H

#include "memory.h"

#define STR_TEMP_BUFFER_SIZE 1024

#define str_append(TXT, FORMAT, ...) \
    do { \
        char _buffer_[STR_TEMP_BUFFER_SIZE]; \
        int _old_len_, _new_len_; \
        char *_new_text_; \
        _new_len_ = sprintf(_buffer_, FORMAT, ##__VA_ARGS__); \
        if (_new_len_ >= (STR_TEMP_BUFFER_SIZE) - 1) { \
            LOG_ERROR("Memory corruption while appending string."); \
            exit(2); \
        } \
        if (!(TXT)) { \
            _new_text_ = (char *)mem_malloc(_new_len_ + 1); \
            memcpy(_new_text_, _buffer_, _new_len_ + 1); \
        } else { \
            _old_len_ = strlen((TXT)); \
            _new_text_ = (char *)mem_malloc(_old_len_ + _new_len_ + 1); \
            memcpy(_new_text_, (TXT), _old_len_); \
            memcpy(_new_text_ + _old_len_, _buffer_, _new_len_ + 1); \
            mem_free((TXT)); \
        } \
        (TXT) = _new_text_; \
    } while(0)

#endif
