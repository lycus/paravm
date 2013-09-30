#include <glib.h>

#include "atom.h"

static void str_free(void *str)
{
    g_free(str);
}

static void int64_free(void *val)
{
    g_free(val);
}

ParaVMAtomTable *paravm_create_atom_table(void)
{
    ParaVMAtomTable *tab = g_new(ParaVMAtomTable, 1);

    tab->str_to_atom = g_hash_table_new_full(&g_str_hash, &g_str_equal, &str_free, &int64_free);
    tab->atom_to_str = g_hash_table_new_full(&g_int64_hash, &g_int64_equal, &int64_free, &str_free);

    tab->mutex = g_new(GMutex, 1);
    g_mutex_init(tab->mutex);

    tab->next_id = 0;
    tab->reuse_queue = g_queue_new();

    return tab;
}

void paravm_destroy_atom_table(ParaVMAtomTable *table)
{
    assert(table);

    if (table)
    {
        g_hash_table_destroy(table->str_to_atom);
        g_hash_table_destroy(table->atom_to_str);

        g_mutex_clear(table->mutex);
        g_free(table->mutex);

        g_queue_free_full(table->reuse_queue, &int64_free);
    }

    g_free((ParaVMAtomTable *)table);
}

ParaVMAtom paravm_string_to_atom(ParaVMAtomTable *table, const char *str)
{
    assert(table);
    assert(str);

    g_mutex_lock(table->mutex);

    ParaVMAtom *existing = g_hash_table_lookup(table->str_to_atom, str);

    if (existing)
    {
        g_mutex_unlock(table->mutex);
        return *existing;
    }

    ParaVMAtom *slot = g_queue_pop_head(table->reuse_queue);

    if (!slot)
    {
        slot = g_new(ParaVMAtom, 1);
        *slot = table->next_id++;
    }

    g_hash_table_insert(table->str_to_atom, g_strdup(str), slot);
    g_hash_table_insert(table->atom_to_str, g_memdup(slot, sizeof(ParaVMAtom)), g_strdup(str));

    g_mutex_unlock(table->mutex);

    return 0;
}

const char *paravm_atom_to_string(const ParaVMAtomTable *table, ParaVMAtom atom)
{
    assert(table);

    g_mutex_lock(table->mutex);

    const char *str = g_hash_table_lookup(table->atom_to_str, &atom);

    g_mutex_unlock(table->mutex);

    return str;
}

void paravm_erase_atom(const ParaVMAtomTable *table, ParaVMAtom atom)
{
    assert(table);

    g_mutex_lock(table->mutex);

    const char *str = g_hash_table_lookup(table->atom_to_str, &atom);

    if (str)
    {
        g_hash_table_remove(table->str_to_atom, str);
        g_hash_table_remove(table->atom_to_str, &atom);

        ParaVMAtom *slot = g_new(ParaVMAtom, 1);
        *slot = atom;

        g_queue_push_tail(table->reuse_queue, slot);
    }

    g_mutex_unlock(table->mutex);
}

void paravm_clear_atoms(ParaVMAtomTable *table)
{
    assert(table);

    g_mutex_lock(table->mutex);

    g_hash_table_remove_all(table->atom_to_str);
    g_hash_table_remove_all(table->str_to_atom);

    table->next_id = 0;
    g_queue_clear(table->reuse_queue);

    g_mutex_unlock(table->mutex);
}
