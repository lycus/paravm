#pragma once

#include "ir.h"

paravm_begin

/* Writes `mod` to `path` in the textual PVA (Parallella
 * Virtual Assembly) format. The file is always overwritten
 * if it exists.
 *
 * This function can return any of the following errors:
 *
 * * `PARAVM_ERROR_READ_ONLY`
 * * `PARAVM_ERROR_ACCESS`
 * * `PARAVM_ERROR_BUSY`
 * * `PARAVM_ERROR_IO_INTERRUPT`
 * * `PARAVM_ERROR_DIRECTORY`
 * * `PARAVM_ERROR_LIMIT`
 * * `PARAVM_ERROR_PATH_LENGTH`
 * * `PARAVM_ERROR_NONEXISTENT`
 * * `PARAVM_ERROR_NO_SPACE`
 * * `PARAVM_ERROR_NOT_DIRECTORY`
 * * `PARAVM_ERROR_IO`
 *
 * If the function succeeds, `PARAVM_ERROR_OK` is returned.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_disassemble_module(const ParaVMModule *mod, const char *path);

paravm_end
