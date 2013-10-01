#pragma once

#include "common.h"

paravm_begin

typedef struct ParaVMAtomTable ParaVMAtomTable;

/* Represents an atom table that maps strings to atoms
 * and atoms to strings. Supports recycling of erased
 * atom IDs.
 */
struct ParaVMAtomTable
{
    void *str_to_atom; // Private. Do not use.
    void *atom_to_str; // Private. Do not use.
    void *mutex; // Private. Do not use.
    uint64_t next_id; // Private. Do not use.
    void *reuse_queue; // Private. Do not use.
};

/* Creates an atom table.
 *
 * Returns a pointer to a `ParaVMAtomTable` instance.
 */
paravm_api
paravm_nothrow
ParaVMAtomTable *paravm_create_atom_table(void);

/* Destroys `table` if it is not `NULL`.
 */
paravm_api
paravm_nothrow
void paravm_destroy_atom_table(ParaVMAtomTable *table);

/* Gets the atom associated with `str` in `table`.
 * If there's no entry in `table` matching `str`, one
 * will be created.
 *
 * Returns the resulting atom.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_string_to_atom(ParaVMAtomTable *table, const char *str);

/* Gets the string associated with `atom` in `table`.
 *
 * Returns the string associated with `atom`, or `NULL`
 * if not matching string could be found. The returned
 * string is bound to the lifetime of the atom, so it
 * must be duplicated if it outlives the atom or the
 * atom table.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const char *paravm_atom_to_string(const ParaVMAtomTable *table, size_t atom);

/* Erases `atom` from `table`. Be careful with this
 * operation as erasing atoms that are in use can
 * result in unpredictable behavior. If `atom` does
 * not exist in the table, no action is taken.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
void paravm_erase_atom(const ParaVMAtomTable *table, size_t atom);

/* Clears `table` of all atoms. Be careful with this
 * operation as erasing atoms that are in use can
 * result in unpredictable behavior.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
void paravm_clear_atoms(ParaVMAtomTable *table);

paravm_end
