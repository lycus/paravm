#pragma once

#include "ir.h"

paravm_begin

extern const uint32_t version; // The most recent module version that this VM supports.

/* Writes `mod` to `path` in the binary PVC (Parallella
 * Virtual Code) format. The file is always overwritten
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
ParaVMError paravm_write_module(const ParaVMModule *mod, const char *path);

/* Reads a binary PVC (Parallella Virtual Code) file from
 * `path` and loads it into `mod`.
 *
 * This function can return any of the following errors:
 *
 * * `PARAVM_ERROR_ACCESS`
 * * `PARAVM_ERROR_IO_INTERRUPT`
 * * `PARAVM_ERROR_DIRECTORY`
 * * `PARAVM_ERROR_LIMIT`
 * * `PARAVM_ERROR_PATH_LENGTH`
 * * `PARAVM_ERROR_NONEXISTENT`
 * * `PARAVM_ERROR_NOT_DIRECTORY`
 * * `PARAVM_ERROR_IO`
 *
 * Additionally, `PARAVM_ERROR_VERSION` is returned if the
 * module's version is too new for this VM to understand.
 * `PARAVM_ERROR_NAME_EXISTS` may be returned if the file
 * contains duplicate definitions of various elements such
 * as functions, registers, basic blocks, etc. Finally,
 * `PARAVM_ERROR_NONEXISTENT_NAME` may be returned if the
 * module contains references to elements that don't exist.
 *
 * If this function fails, `mod` will be in an intermediate
 * stage of construction which is most likely undesirable,
 * so it should be passed to `paravm_destroy_module`.
 *
 * If the function succeeds, `PARAVM_ERROR_OK` is returned.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_read_module(const char *path, const ParaVMModule *mod);

/* Extracts the module name from `path`. For instance, a
 * path such as `/foo/bar/baz.pvc` would have the module
 * name `baz`. The returned pointer should be freed with
 * `free`.
 *
 * If `path` doesn't end in `.pva` or `.pvc`, a `NULL`
 * pointer is returned.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
char *paravm_extract_module_name(const char *path);

paravm_end
