/*
 *
 * Author: emadflash
 * License: GPL-v3
 * Date: Thursday 04 November 2021
 *
 */
#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

/*
 * String
 */
typedef struct {
    char* data;
    size_t len;
} String;

extern String string_alloc(char* str);
extern inline void string_free(String* string);

/*
 * String_Vec
 */
typedef struct {
    String* data;
    size_t len;
} String_Vec;

#endif
