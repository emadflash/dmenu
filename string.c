/*
 *
 * Author: emadflash
 * License: GPL-v3
 * Date: Thursday 04 November 2021
 *
 */
#include "string.h"

#include <stdlib.h>
#include <string.h>

String
string_alloc(char* str) {
    String string;

    string.len = strlen(str);
    string.data = (char*) malloc(string.len + 1);
    strcpy(string.data, str);

    return string;
}

inline void
string_free(String* string) {
    free(string->data);
}
