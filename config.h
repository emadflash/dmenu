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
#include <stdint.h>
#include <stdbool.h>

#include "string.h"
#include "external/lua/include/lua.h"
#include "external/lua/include/lualib.h"
#include "external/lua/include/lauxlib.h"

#define lua_checkType(X, Y, Z) ((lua_type(X, Y) != Z) ? -1 : 0)
#define strcmp_withoutCase(X, Y) (strcasecmp(X, Y) == 0)
#define config_colorsGetCArray(X, Y) { (X).Colors.Y[0].data, (X).Colors.Y[1].data }

/*
 * ConfigResult
 */
typedef enum {
    ConfigResult_SUCCESS,
    ConfigResult_InvaildOption,
    ConfigResult_MissingOption,
    ConfigResult_FailedToRead,
    ConfigResult_MainDmenuCTxNotTable,
} ConfigResult;

extern const char* ConfigResult_toString(const ConfigResult result);

/*
 * Config
 */
static const char* keys[6] = { "topbar", "fonts", "prompt", "colors", "lines", "worddelimiters" };
typedef struct {
    uint8_t topbar;

    struct {
        String* fonts;
        size_t len;
        bool is_fonts;
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

extern inline void config_set_firstFont(Config* config, char* font);
extern inline void config_set_prompt(Config* config, char* prompt);
extern inline void config_set_ColorsSchemeNorm(Config* config, int idx, const char* color);
extern inline void config_set_ColorsSchemeSel(Config* config, int idx, const char* color);
extern inline void config_set_ColorsSchemeOut(Config* config, int idx, const char* color);

extern void config_free(Config* config);
extern ConfigResult config_parse(Config* config, lua_State* lua);
extern ConfigResult read_config(Config* config, const char* fname);

#endif
