/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef STRBUILD_H
#define STRBUILD_H

#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "log.h"

#define STR_TEMP_BUFFER_SIZE 1024 * 10

#define str_append(TXT, FORMAT, ...)\
    do {\
        char *_buffer_; \
        int _old_len_, _new_len_;\
        char *_new_text_;\
    _buffer_ = mem_malloc(STR_TEMP_BUFFER_SIZE); \
        _new_len_ = sprintf(_buffer_, FORMAT, ##__VA_ARGS__);\
        if (_new_len_ >= (STR_TEMP_BUFFER_SIZE) - 1) {\
            LOG_ERROR("Buffer of %d not enough for %d bytes.", \
                STR_TEMP_BUFFER_SIZE, \
        _new_len_); \
            exit(2);\
        }\
        if (!(TXT)) {\
            _new_text_ = (char *)mem_malloc(_new_len_ + 1);\
            memcpy(_new_text_, _buffer_, _new_len_ + 1);\
        } else {\
            _old_len_ = strlen((TXT));\
            _new_text_ = (char *)mem_malloc(_old_len_ + _new_len_ + 1); \
            memcpy(_new_text_, (TXT), _old_len_);\
            memcpy(_new_text_ + _old_len_, _buffer_, _new_len_ + 1);\
            mem_free((TXT));\
        } \
        (TXT) = _new_text_; \
    mem_free(_buffer_); \
    } while(0)

#define str_append_range(TXT, FIRST, LAST)\
    do {\
        int _old_len_, _new_len_;\
        char *_new_text_;\
        _new_len_ = ((LAST) - (FIRST));\
        if (!(TXT)) {\
            _new_text_ = (char *) mem_malloc(_new_len_ + 1);\
            memcpy(_new_text_, (FIRST), _new_len_);\
            _new_text_[_new_len_] = '\0';\
        } else {\
            _old_len_ = strlen((TXT));\
            _new_text_ = (char *)mem_malloc(_old_len_ + _new_len_ + 1);\
            memcpy(_new_text_, (TXT), _old_len_);\
            memcpy(_new_text_ + _old_len_, (FIRST), _new_len_);\
            _new_text_[_old_len_ + _new_len_] = '\0';\
            mem_free((TXT));\
        }\
        (TXT) = _new_text_;\
    } while(0)

#endif
