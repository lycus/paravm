#include <glib.h>

#include "ir.h"

const ParaVMRegister *paravm_create_register(const char *name, bool argument)
{
    assert(name);

    ParaVMRegister *r = g_new(ParaVMRegister, 1);

    r->name = g_strdup(name);
    r->argument = argument;

    return r;
}

void paravm_destroy_register(const ParaVMRegister *reg)
{
    if (reg)
        g_free((char *)reg->name);

    g_free((ParaVMRegister *)reg);
}

const ParaVMInstruction *paravm_create_instruction(const ParaVMOpCode *op,
                                                   ParaVMOperand operand,
                                                   bool own_operand,
                                                   const ParaVMRegister *reg1,
                                                   const ParaVMRegister *reg2,
                                                   const ParaVMRegister *reg3)
{
    assert(op);

    if (op->operand != PARAVM_OPERAND_TYPE_NONE)
        assert(operand.raw);

    if (op->operand == PARAVM_OPERAND_TYPE_BLOCKS)
        assert(operand.blocks[1]);

    if (op->registers >= 1)
        assert(reg1);

    if (op->registers >= 2)
        assert(reg2);

    if (op->registers >= 3)
        assert(reg3);

    ParaVMInstruction *i = g_new(ParaVMInstruction, 1);

    i->opcode = op;

    if (op->operand == PARAVM_OPERAND_TYPE_BLOCK ||
        op->operand == PARAVM_OPERAND_TYPE_BLOCKS)
    {
        // For these operand types, never assume ownership.
        i->operand = operand;
        i->own_operand = false;
    }
    else if (own_operand)
    {
        if (op->operand == PARAVM_OPERAND_TYPE_ARGS)
        {
            const char *const *arr = operand.args;
            size_t len = 0;

            // Not ideal, but eh...
            for (const char *const *p = arr; *p; p++)
                len++;

            const char **str_arr = g_new(const char *, len);

            for (const char *const *src = arr, **dst = str_arr; *src; src++, dst++)
                *dst = g_strdup(*src);

            i->operand.args = str_arr;
        }
        else
            i->operand.string = g_strdup(operand.string);

        i->own_operand = true;
    }
    else
    {
        i->operand = operand;
        i->own_operand = false;
    }

    i->register1 = reg1;
    i->register2 = reg2;
    i->register3 = reg3;

    return i;
}

void paravm_destroy_instruction(const ParaVMInstruction *insn)
{
    if (insn && insn->own_operand)
    {
        if (insn->opcode->operand == PARAVM_OPERAND_TYPE_ARGS)
            for (const char *const *ptr = insn->operand.args; *ptr; ptr++)
                g_free(*(void **)ptr);

        g_free((void *)insn->operand.raw);
    }

    g_free((ParaVMInstruction *)insn);
}

const ParaVMBlock *paravm_create_block(const char *name)
{
    assert(name);

    ParaVMBlock *b = g_new(ParaVMBlock, 1);

    b->name = g_strdup(name);
    b->handler = null;
    b->exception = null;

    b->instruction_list = g_array_new(true, false, sizeof(ParaVMInstruction *));

    return b;
}

void paravm_destroy_block(const ParaVMBlock *block)
{
    if (block)
    {
        g_free((char *)block->name);

        GArray *arr = (GArray *)block->instruction_list;
        ParaVMInstruction **insn;

        for (insn = &g_array_index(arr, ParaVMInstruction *, 0); *insn; insn++)
            paravm_destroy_instruction(*insn);

        g_array_free(arr, true);
    }

    g_free((ParaVMBlock *)block);
}

ParaVMError paravm_set_handler_block(const ParaVMBlock *block, const ParaVMBlock *handler)
{
    assert(block);

    if (block->handler)
        return PARAVM_ERROR_ALREADY_SET;

    ((ParaVMBlock *)block)->handler = handler;

    return PARAVM_ERROR_OK;
}

ParaVMError paravm_set_exception_register(const ParaVMBlock *block, const ParaVMRegister *exception)
{
    assert(block);

    if (block->exception)
        return PARAVM_ERROR_ALREADY_SET;

    ((ParaVMBlock *)block)->exception = exception;

    return PARAVM_ERROR_OK;
}

void paravm_prepend_instruction(const ParaVMBlock *block, const ParaVMInstruction *insn)
{
    assert(block);
    assert(insn);

    g_array_prepend_val((GArray *)block->instruction_list, insn);
}

void paravm_append_instruction(const ParaVMBlock *block, const ParaVMInstruction *insn)
{
    assert(block);
    assert(insn);

    g_array_append_val((GArray *)block->instruction_list, insn);
}

const ParaVMInstruction *paravm_get_instruction(const ParaVMBlock *block, size_t idx)
{
    assert(block);

    GArray *arr = (GArray *)block->instruction_list;

    if (idx >= arr->len)
        return null;

    return g_array_index(arr, ParaVMInstruction *, idx);
}

size_t paravm_get_instruction_index(const ParaVMBlock *block, const ParaVMInstruction *insn)
{
    assert(block);
    assert(insn);

    GArray *arr = (GArray *)block->instruction_list;

    for (size_t i = 0; i < arr->len; i++)
    {
        const ParaVMInstruction *e = g_array_index(arr, const ParaVMInstruction *, i);

        if (e == insn)
            return i;
    }

    return SIZE_MAX;
}

const ParaVMInstruction *const *paravm_get_instructions(const ParaVMBlock *block)
{
    assert(block);

    return (const ParaVMInstruction *const *)((GArray *)block->instruction_list)->data;
}

size_t paravm_get_instruction_count(const ParaVMBlock *block)
{
    assert(block);

    return ((GArray *)block->instruction_list)->len;
}

const ParaVMFunction *paravm_create_function(const char *name)
{
    assert(name);

    ParaVMFunction *f = g_new(ParaVMFunction, 1);

    f->name = g_strdup(name);

    f->argument_table = g_hash_table_new(&g_str_hash, &g_str_equal);
    f->argument_list = g_array_new(true, false, sizeof(const ParaVMRegister *));
    f->register_table = g_hash_table_new_full(&g_str_hash, &g_str_equal, null,
                                              (GDestroyNotify)&paravm_destroy_register);
    f->register_list = g_array_new(true, false, sizeof(const ParaVMRegister *));
    f->block_table = g_hash_table_new_full(&g_str_hash, &g_str_equal, null,
                                           (GDestroyNotify)&paravm_destroy_block);
    f->block_list = g_array_new(true, false, sizeof(const ParaVMBlock *));

    return f;
}

void paravm_destroy_function(const ParaVMFunction *func)
{
    if (func)
    {
        g_free((char *)func->name);

        g_hash_table_destroy((GHashTable *)func->argument_table);
        g_array_free((GArray *)func->argument_list, true);
        g_hash_table_destroy((GHashTable *)func->register_table);
        g_array_free((GArray *)func->register_list, true);
        g_hash_table_destroy((GHashTable *)func->block_table);
        g_array_free((GArray *)func->block_list, true);
    }

    g_free((ParaVMFunction *)func);
}

ParaVMError paravm_add_block(const ParaVMFunction *func, const ParaVMBlock *block)
{
    assert(func);
    assert(block);

    if (g_hash_table_lookup((GHashTable *)func->block_table, block->name))
        return PARAVM_ERROR_NAME_EXISTS;

    g_hash_table_insert((GHashTable *)func->block_table, (char *)block->name,
                        (ParaVMBlock *)block);
    g_array_append_val((GArray *)func->block_list, block);

    return PARAVM_ERROR_OK;
}

const ParaVMBlock *paravm_get_block(const ParaVMFunction *func, const char *name)
{
    assert(func);
    assert(name);

    return g_hash_table_lookup((GHashTable *)func->block_table, name);
}

const ParaVMBlock *const *paravm_get_blocks(const ParaVMFunction *func)
{
    assert(func);

    return (const ParaVMBlock *const *)((GArray *)func->block_list)->data;
}

size_t paravm_get_block_count(const ParaVMFunction *func)
{
    assert(func);

    return ((GArray *)func->block_list)->len;
}

ParaVMError paravm_add_register(const ParaVMFunction *func, const ParaVMRegister *reg)
{
    assert(func);
    assert(reg);

    if (g_hash_table_lookup((GHashTable *)func->register_table, reg->name))
        return PARAVM_ERROR_NAME_EXISTS;

    g_hash_table_insert((GHashTable *)func->register_table,
                        (char *)reg->name, (ParaVMRegister *)reg);
    g_array_append_val((GArray *)func->register_list, reg);

    if (reg->argument)
    {
        g_hash_table_insert((GHashTable *)func->argument_table,
                            (char *)reg->name, (ParaVMRegister *)reg);
        g_array_append_val((GArray *)func->argument_list, reg);
    }

    return PARAVM_ERROR_OK;
}

const ParaVMRegister *paravm_get_register(const ParaVMFunction *func, const char *name)
{
    assert(func);
    assert(name);

    return g_hash_table_lookup((GHashTable *)func->register_table, name);
}

const ParaVMRegister *const *paravm_get_registers(const ParaVMFunction *func)
{
    assert(func);

    return (const ParaVMRegister *const *)((GArray *)func->register_list)->data;
}

size_t paravm_get_register_count(const ParaVMFunction *func)
{
    assert(func);

    return ((GArray *)func->register_list)->len;
}

const ParaVMRegister *paravm_get_argument(const ParaVMFunction *func, const char *name)
{
    assert(func);
    assert(name);

    return g_hash_table_lookup((GHashTable *)func->argument_table, name);
}

const ParaVMRegister *const *paravm_get_arguments(const ParaVMFunction *func)
{
    assert(func);

    return (const ParaVMRegister *const *)((GArray *)func->argument_list)->data;
}

size_t paravm_get_argument_count(const ParaVMFunction *func)
{
    assert(func);

    return ((GArray *)func->argument_list)->len;
}

const ParaVMModule *paravm_create_module(const char *name)
{
    assert(name);

    ParaVMModule *m = g_new(ParaVMModule, 1);

    m->name = g_strdup(name);

    m->function_table = g_hash_table_new_full(&g_str_hash, &g_str_equal, null,
                                              (GDestroyNotify)&paravm_destroy_function);
    m->function_list = g_array_new(true, false, sizeof(const ParaVMFunction *));

    return m;
}

void paravm_destroy_module(const ParaVMModule *mod)
{
    if (mod)
    {
        g_free((char *)mod->name);

        g_hash_table_destroy((GHashTable *)mod->function_table);
        g_array_free((GArray *)mod->function_list, true);
    }

    g_free((ParaVMModule *)mod);
}

ParaVMError paravm_add_function(const ParaVMModule *mod, const ParaVMFunction *func)
{
    assert(mod);
    assert(func);

    if (g_hash_table_lookup((GHashTable *)mod->function_table, func->name))
        return PARAVM_ERROR_NAME_EXISTS;

    g_hash_table_insert((GHashTable *)mod->function_table,
                        (char *)func->name, (ParaVMFunction *)func);
    g_array_append_val((GArray *)mod->function_list, func);

    return PARAVM_ERROR_OK;
}

const ParaVMFunction *paravm_get_function(const ParaVMModule *mod, const char *name)
{
    assert(mod);
    assert(name);

    return g_hash_table_lookup((GHashTable *)mod->function_table, name);
}

const ParaVMFunction *const *paravm_get_functions(const ParaVMModule *mod)
{
    assert(mod);

    return (const ParaVMFunction *const *)((GArray *)mod->function_list)->data;
}

size_t paravm_get_function_count(const ParaVMModule *mod)
{
    assert(mod);

    return ((GArray *)mod->function_list)->len;
}
