#pragma once

#include "error.h"

paravm_begin

typedef enum ParaVMTokenType ParaVMTokenType;

/* Specifies the type of a token.
 */
enum ParaVMTokenType
{
    PARAVM_TOKEN_TYPE_STRING = 0, // A string such as `"foo"`.
    PARAVM_TOKEN_TYPE_ATOM = 1, // An atom such as `'foo'`.
    PARAVM_TOKEN_TYPE_INTEGER = 2, // An integer such as `123`.
    PARAVM_TOKEN_TYPE_FLOAT = 3, // A float such as `123.321`.
    PARAVM_TOKEN_TYPE_BINARY = 4, // A binary such as `:11011011011001101:`.
    PARAVM_TOKEN_TYPE_FUN = 5, // The `.fun` directive.
    PARAVM_TOKEN_TYPE_ARG = 6, // The `.arg` directive.
    PARAVM_TOKEN_TYPE_REG = 7, // The `.reg` directive.
    PARAVM_TOKEN_TYPE_BLK = 8, // The `.blk` directive.
    PARAVM_TOKEN_TYPE_UNW = 9, // The `.unw` directive.
    PARAVM_TOKEN_TYPE_PAREN_OPEN = 10, // The opening parenthesis, `(`.
    PARAVM_TOKEN_TYPE_PAREN_CLOSE = 11, // The closing parenthesis, `)`.
    PARAVM_TOKEN_TYPE_BRACKET_OPEN = 12, // The opening bracket, `[`.
    PARAVM_TOKEN_TYPE_BRACKET_CLOSE = 13, // The closing bracket, `]`.
    PARAVM_TOKEN_TYPE_OPCODE = 14, // An opcode such as `mov`.
};

typedef struct ParaVMToken ParaVMToken;

/* Defines a token as produced by the lexer.
 */
struct ParaVMToken
{
    ParaVMTokenType type; // The type of the token.
    char *value; // The string value (excluding quotes for strings and atoms).
    uint32_t line; // The line this token is on.
    uint32_t column; // The column this token is at.
};

/* Creates a new `ParaVMToken` with the given values. The
 * `value` string is copied, so it needs to be freed by the
 * caller.
 *
 * Returns a `ParaVMToken` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMToken *paravm_create_token(ParaVMTokenType type, char *value,
                                 uint32_t line, uint32_t column);

/* Destroys `tok` if it is not `NULL`.
 */
paravm_api
paravm_nothrow
void paravm_destroy_token(ParaVMToken *tok);

/* Attempts to lex `str` as PVA source code. The function sets
 * `*tokens` to `NULL`, `*line` to `1`, and `*column` to 0. It
 * then proceeds to process the string. If no error occurred,
 * `*tokens` is set to an array with the lexed tokens. The
 * array will be terminated with a `NULL` pointer. In the case
 * of an error, `*line` and `*column` will be set to the
 * location where the error approximately occurred.
 *
 * Each token in `tokens` should be freed with the
 * `paravm_destroy_token` function, and the array itself with
 * `free`.
 *
 * Returns `PARAVM_ERROR_BAD_UTF8` if the input is malformed,
 * `PARAVM_ERROR_SYNTAX` if there's a syntax error,
 * `PARAVM_ERROR_OVERFLOW` if a floating point value was too
 * large, or `PARAVM_ERROR_OK` if lexing the string was
 * successful.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_lex_string(const char *str, ParaVMToken ***tokens,
                              uint32_t *line, uint32_t *column);

paravm_end
