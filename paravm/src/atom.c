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

    tab->rw_lock = g_new(GRWLock, 1);
    g_rw_lock_init(tab->rw_lock);

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

        g_rw_lock_clear(table->rw_lock);
        g_free(table->rw_lock);

        g_queue_free_full(table->reuse_queue, &int64_free);
    }

    g_free((ParaVMAtomTable *)table);
}

size_t paravm_string_to_atom(ParaVMAtomTable *table, const char *str)
{
    assert(table);
    assert(str);

    g_rw_lock_reader_lock(table->rw_lock);

    size_t *existing = g_hash_table_lookup(table->str_to_atom, str);

    if (existing)
    {
        g_rw_lock_reader_unlock(table->rw_lock);
        return *existing;
    }

    g_rw_lock_writer_lock(table->rw_lock);

    size_t *slot = g_queue_pop_head(table->reuse_queue);

    if (!slot)
    {
        slot = g_new(size_t, 1);
        *slot = table->next_id++;
    }

    g_hash_table_insert(table->str_to_atom, g_strdup(str), slot);
    g_hash_table_insert(table->atom_to_str, g_memdup(slot, sizeof(size_t)), g_strdup(str));

    g_rw_lock_writer_unlock(table->rw_lock);

    return 0;
}

const char *paravm_atom_to_string(const ParaVMAtomTable *table, size_t atom)
{
    assert(table);

    g_rw_lock_reader_lock(table->rw_lock);

    const char *str = g_hash_table_lookup(table->atom_to_str, &atom);

    g_rw_lock_reader_unlock(table->rw_lock);

    return str;
}

void paravm_erase_atom(const ParaVMAtomTable *table, size_t atom)
{
    assert(table);

    g_rw_lock_writer_lock(table->rw_lock);

    const char *str = g_hash_table_lookup(table->atom_to_str, &atom);

    if (str)
    {
        g_hash_table_remove(table->str_to_atom, str);
        g_hash_table_remove(table->atom_to_str, &atom);

        size_t *slot = g_new(size_t, 1);
        *slot = atom;

        g_queue_push_tail(table->reuse_queue, slot);
    }

    g_rw_lock_writer_unlock(table->rw_lock);
}

void paravm_clear_atoms(ParaVMAtomTable *table)
{
    assert(table);

    g_rw_lock_writer_lock(table->rw_lock);

    g_hash_table_remove_all(table->atom_to_str);
    g_hash_table_remove_all(table->str_to_atom);

    table->next_id = 0;
    g_queue_clear(table->reuse_queue);

    g_rw_lock_writer_unlock(table->rw_lock);
}
