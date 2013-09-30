#pragma once

#include "ir.h"
#include "lexer.h"

paravm_begin

/* Assembles a module from the source code represented by
 * `tokens`. The function sets `*line` to `1` and `*column`
 * to `0`. It then proceeds to process the tokens. If no
 * error occurred, `mod` will be the assembled module. In
 * the case of an error, `*line` and `*column` will be set
 * to the location where the error approximately occurred,
 * and `mod` should be treated as garbage and immediately
 * be passed to `paravm_destroy_module`.
 *
 * Returns `PARAVM_ERROR_SYNTAX` if there was some kind of
 * error in the order of the input tokens. Returns
 * `PARAVM_ERROR_ASSEMBLY` if something went wrong while
 * constructing the module. Otherwise, `PARAVM_ERROR_OK` is
 * returned.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_assemble_tokens(ParaVMToken **tokens, const ParaVMModule *mod,
                                   uint32_t *line, uint32_t *column);

paravm_end
