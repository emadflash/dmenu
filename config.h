/*
 *
 * Author: emadflash
 * License: GPL-v3
 * Date: Thursday 04 November 2021
 *
 */
#ifndef __CONFIG_H__ 
#define __CONFIG_H__ 

#include <stdio.h>
#include <string.h>

#include "string.h"
#include "external/lua/include/lua.h"
#include "external/lua/include/lualib.h"
#include "external/lua/include/lauxlib.h"

#define lua_checkType(X, Y, Z) ((lua_type(X, Y) != Z) ? -1 : 0)
#define strcmp_withoutCase(X, Y) (strcasecmp(X, Y) == 0)
#define config_colorsGetCArray(X, Y) { (X).Colors.Y[0].data, (X).Colors.Y[1].data }

static const char* keys[6] = { "topbar", "fonts", "prompt", "colors", "lines", "worddelimiters" };

typedef struct {
    uint8_t topbar;

    struct {
        String* fonts;
        size_t len;
    } Fonts;

    String prompt;

    struct {
        String SchemeNorm[2];
        String SchemeSel[2];
        String SchemeOut[2];
    } Colors;

    uint16_t lines;
    String worddelimiters;
} Config;

extern void config_free(Config* config);
extern void config_parseFonts(Config* config, lua_State* lua, int idx);
extern void config_parseColors(Config* config, lua_State* lua, int idx);
extern Config config_parse(lua_State* lua);
extern int read_config(Config* config);

#endif
