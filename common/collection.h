/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef COLLECTION_H
#define COLLECTION_H

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
        mem_free((MACRO_ARRAY).data); \
        (MACRO_ARRAY).data = NULL; \
        (MACRO_ARRAY).size = 0; \
        (MACRO_ARRAY).cap = 0; \
    } while(0)

#define ARRAY_COPY(dst, src) \
    do { \
        int size = src.cap * sizeof(*dst.data); \
		dst.data = mem_malloc(size); \
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
			(MACRO_ARRAY).data = mem_malloc(sizeof(*((MACRO_ARRAY).data))); \
            (MACRO_ARRAY).cap = 1; \
        } else if ((MACRO_ARRAY).cap == (MACRO_ARRAY).size) { \
            (MACRO_ARRAY).cap *= 2; \
			(MACRO_ARRAY).data = mem_realloc(\
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


#endif
