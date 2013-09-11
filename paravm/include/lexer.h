#pragma once

#include "common.h"
#include "error.h"

typedef enum {
    PARAVM_TOKEN_TYPE_COMMENT = 0,
    PARAVM_TOKEN_TYPE_STRING = 1,
    PARAVM_TOKEN_TYPE_ATOM = 2,
    PARAVM_TOKEN_TYPE_INTEGER = 3,
    PARAVM_TOKEN_TYPE_FLOAT = 4,
    PARAVM_TOKEN_TYPE_MOD = 5,
    PARAVM_TOKEN_TYPE_GLB = 6,
    PARAVM_TOKEN_TYPE_LOC = 7,
    PARAVM_TOKEN_TYPE_FUN = 8,
    PARAVM_TOKEN_TYPE_ARG = 9,
    PARAVM_TOKEN_TYPE_REG = 10,
    PARAVM_TOKEN_TYPE_BLK = 11,
    PARAVM_TOKEN_TYPE_PAREN_OPEN = 12,
    PARAVM_TOKEN_TYPE_PAREN_CLOSE = 13,
    PARAVM_TOKEN_TYPE_BRACKET_OPEN = 14,
    PARAVM_TOKEN_TYPE_BRACKET_CLOSE = 15,
    PARAVM_TOKEN_TYPE_SEMICOLON = 16,
    PARAVM_TOKEN_TYPE_OPCODE = 17,
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
