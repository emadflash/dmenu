/*
 *
 * Author: emadflash
 * License: GPL-v3
 * Date: Thursday 04 November 2021
 *
 */
#include "config.h"

#include <stdlib.h>
#include <assert.h>

const char*
ConfigResult_toString(const ConfigResult result) {
#define to_string(X, Y)\
    case (X):          \
        return (Y)
    switch (result) {
        to_string(ConfigResult_SUCCESS, "Success");
        to_string(ConfigResult_InvaildOption, "Encountered invaild option");
        to_string(ConfigResult_MissingOption, "Missing required option");
        to_string(ConfigResult_FailedToRead, "Failed to read file");
        to_string(ConfigResult_MainDmenuCTxNotTable, "Main dmenu ctx in config.lua should be a table");
    }

    return "???";
}

static inline uint8_t
check_missingKey(const int* is_visit_fields, uint8_t len) {
    for (uint8_t i = 0; i < len; ++i) {
        if (is_visit_fields[i] != 1) return i;
    }

    return 0;
}

void
config_free(Config* config) {
#define config_freeColorsBatch(X)       \
    string_free(&(config->Colors.X[0]));\
    string_free(&(config->Colors.X[1]))

    /* Free Fonts
     */
    if (config->Fonts.is_fonts) {
        for (size_t i = 0; i < config->Fonts.len; ++i) {
            string_free(&(config->Fonts.fonts[i]));
        }

        free(config->Fonts.fonts);
    }

     /* Free prompt
      */
    string_free(&(config->prompt));

    /* Free Colors
     */
    config_freeColorsBatch(SchemeNorm);
    config_freeColorsBatch(SchemeSel);
    config_freeColorsBatch(SchemeOut);

     /* Free worddelimiters
      */
    string_free(&(config->worddelimiters));
}

void
config_parseFonts(Config* config, lua_State* lua, int idx) {
    config->Fonts.len = 5;
    config->Fonts.fonts = (String*) malloc(sizeof(String) * config->Fonts.len);

    size_t entries_counter = 0;

    lua_pushnil(lua);
    while ((lua_next(lua, idx - 1)) != 0) {
        if (entries_counter >= config->Fonts.len) {
            config->Fonts.len += 10;
            config->Fonts.fonts = (String*) realloc(config->Fonts.fonts, sizeof(String) * config->Fonts.len);
        }

        luaL_checktype(lua, -1, LUA_TSTRING);
        config->Fonts.fonts[entries_counter++] = string_alloc((char*) lua_tostring(lua, -1));
        lua_pop(lua, 1);
    }

    if (entries_counter == 0) {
        config->Fonts.is_fonts = false;
        config->Fonts.len = 0;
        free(config->Fonts.fonts);

        return;
    }

    config->Fonts.is_fonts = true; 
    config->Fonts.len = entries_counter;
}

void
config_parseColors(Config* config, lua_State* lua, int idx) {
#define config_parseColorsBatch(X)\
    lua_pushnil(lua);                                             \
    while (lua_next(lua, -2) != 0) {                              \
        luaL_checktype(lua, -1, LUA_TSTRING);                     \
        config->Colors.X[0] = string_alloc(lua_tostring(lua, -1));\
                                                                  \
        lua_pop(lua, 1);                                          \
        lua_next(lua, -2);                                        \
                                                                  \
        luaL_checktype(lua, -1, LUA_TSTRING);                     \
        config->Colors.X[1] = string_alloc(lua_tostring(lua, -1));\
        lua_pop(lua, 1);                                          \
    }

    lua_pushnil(lua);
    while ((lua_next(lua, -2)) != 0) {
        luaL_checktype(lua, -1, LUA_TTABLE);
        char* Key = lua_tostring(lua, -2);
        
        if (strcmp_withoutCase(Key, "SchemeNorm")) {
            luaL_checktype(lua, -1, LUA_TTABLE);
            config_parseColorsBatch(SchemeNorm);
        } else if (strcmp_withoutCase(Key, "SchemeSel")) {
            config_parseColorsBatch(SchemeSel);
        } else if (strcmp_withoutCase(Key, "SchemeOut")) {
            config_parseColorsBatch(SchemeOut);
        }

        lua_pop(lua, 1);
    }

    // emf(todo): Check if SchemeNorm, SchemeSel, SchemeOut are all read
}

ConfigResult
config_parse(Config* config, lua_State* lua) {
    int is_visit_fields[6] = {0};

    lua_pushnil(lua);
    while (lua_next(lua, -2) != 0) {
        char* Key = lua_tostring(lua, -2);

        if (strcmp_withoutCase(Key, "topbar") && !is_visit_fields[0]) {
            is_visit_fields[0] = 1;
            luaL_checktype(lua, -1, LUA_TBOOLEAN);

            config->topbar = lua_toboolean(lua, -1);
        } else if (strcmp_withoutCase(Key, "fonts") && !is_visit_fields[1]) {
            is_visit_fields[1] = 1;
            luaL_checktype(lua, -1, LUA_TTABLE);
            
            config_parseFonts(&config, lua, -1);
        } else if (strcmp_withoutCase(Key, "prompt") && !is_visit_fields[2]) {
            is_visit_fields[2] = 1;
            luaL_checktype(lua, -1, LUA_TSTRING);

            config->prompt = string_alloc((char*) lua_tostring(lua, -1));
        } else if (strcmp_withoutCase(Key, "colors") && !is_visit_fields[3]) {
            is_visit_fields[3] = 1;
            luaL_checktype(lua, -1, LUA_TTABLE);

            config_parseColors(config, lua, -1);
        } else if (strcmp_withoutCase(Key, "lines") && !is_visit_fields[4]) {
            is_visit_fields[4] = 1;
            luaL_checktype(lua, -1, LUA_TNUMBER);

            config->lines = lua_tointeger(lua, -1);
        } else if (strcmp_withoutCase(Key, "worddelimiters") && !is_visit_fields[5]) {
            is_visit_fields[5] = 1;
            luaL_checktype(lua, -1, LUA_TSTRING);

            config->worddelimiters = string_alloc(lua_tostring(lua, -1));
        } else return ConfigResult_InvaildOption;

        lua_pop(lua, 1);
    }

    uint8_t missingKey;
    if ((missingKey = check_missingKey(is_visit_fields, 6)) != 0) return ConfigResult_MissingOption;
    return ConfigResult_SUCCESS;
}

ConfigResult
read_config(Config* config, const char* fname) {
    lua_State* lua = luaL_newstate();
    if (luaL_dofile(lua, fname) != LUA_OK) return ConfigResult_FailedToRead;
    
    lua_getglobal(lua, "dmenu");
    if (lua_checkType(lua, -1, LUA_TTABLE) < 0) return ConfigResult_MainDmenuCTxNotTable;

    ConfigResult result = config_parse(config, lua);
    lua_close(lua);

    return (result != ConfigResult_SUCCESS) ? result: ConfigResult_SUCCESS;
}
