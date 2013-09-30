#pragma once

#include "atom.h"

paravm_begin

typedef struct ParaVMContext ParaVMContext;

/* Defines the execution context of a ParaVM instance.
 */
struct ParaVMContext
{
    const char *hdf; // The mesh description file, or `NULL`.
    ParaVMAtomTable *atom_table; // The VM instance's atom table.
};

/* Determines whether ParaVM was compiled with the eSDK.
 *
 * Returns `true` if yes; otherwise, `false`.
 */
paravm_api
paravm_nothrow
bool paravm_has_esdk(void);

/* Attempts to create a ParaVM context. The `hdf` parameter
 * specifies the file to read the Epiphany mesh description
 * from. If it is `NULL`, the context will use the emulator.
 *
 * Returns a valid context if the `hdf` file was opened and
 * read successfully or if `hdf` was `NULL`. Otherwise, if
 * `hdf` could not be read or ParaVM was compiled without
 * the eSDK, returns `NULL`.
 */
paravm_api
paravm_nothrow
ParaVMContext *paravm_create_context(const char *hdf);

/* Frees `context` if it is not `NULL`. Only call this once
 * all ParaVM execution has ceased or the behavior will be
 * unpredictable.
 */
paravm_api
paravm_nothrow
void paravm_destroy_context(ParaVMContext *ctx);

paravm_end
