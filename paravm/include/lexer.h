#pragma once

#include "common.h"
#include "error.h"

typedef enum {
    PARAVM_TOKEN_TYPE_COMMENT = 0,
    PARAVM_TOKEN_TYPE_STRING = 1,
    PARAVM_TOKEN_TYPE_INTEGER = 2,
    PARAVM_TOKEN_TYPE_FLOAT = 3,
    PARAVM_TOKEN_TYPE_TRUE = 4,
    PARAVM_TOKEN_TYPE_FALSE = 5,
    PARAVM_TOKEN_TYPE_MOD = 6,
    PARAVM_TOKEN_TYPE_GLB = 7,
    PARAVM_TOKEN_TYPE_LOC = 8,
    PARAVM_TOKEN_TYPE_FUN = 9,
    PARAVM_TOKEN_TYPE_ARG = 10,
    PARAVM_TOKEN_TYPE_REG = 11,
    PARAVM_TOKEN_TYPE_BLK = 12,
    PARAVM_TOKEN_TYPE_PAREN_OPEN = 13,
    PARAVM_TOKEN_TYPE_PAREN_CLOSE = 14,
    PARAVM_TOKEN_TYPE_BRACKET_OPEN = 15,
    PARAVM_TOKEN_TYPE_BRACKET_CLOSE = 16,
    PARAVM_TOKEN_TYPE_SEMICOLON = 17,
    PARAVM_TOKEN_TYPE_INSTRUCTION = 18,
} ParaVMTokenType;

typedef struct {
    ParaVMTokenType type;
    const char *value;
    uint32_t line;
    uint32_t column;
} ParaVMToken;

paravm_api
paravm_nothrow
paravm_const
paravm_nonnull()
ParaVMError paravm_lex_string(const char *str, ParaVMToken **tokens,
                              uint32_t *line, uint32_t *column);
