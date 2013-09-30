#include <glib.h>

#include "assemble.h"
#include "opcodes.h"

static void free_hash_table(void *ht)
{
    g_hash_table_destroy(ht);
}

static void free_ptr_array(void *arr)
{
    g_ptr_array_free(arr, true);
}

static void free_instruction(void *insn)
{
    const ParaVMInstruction *ins = (const ParaVMInstruction *)insn;

    if (ins->opcode->operand == PARAVM_OPERAND_TYPE_ARGS)
        g_ptr_array_free((GPtrArray *)ins->operand.raw, true);

    paravm_destroy_instruction(insn);
}

ParaVMError paravm_assemble_tokens(ParaVMToken **tokens, const ParaVMModule *mod,
                                   uint32_t *line, uint32_t *column)
{
    assert(tokens);
    assert(mod);
    assert(line);
    assert(column);

    *line = 1;
    *column = 0;

    __block ParaVMToken **ltokens = tokens;

    ParaVMToken *(^ peek_token)(void) = ^(void)
    {
        return *ltokens;
    };

    __block uint32_t lline = 1;
    __block uint32_t lcolumn = 0;

    ParaVMToken *(^ next_token)(void) = ^(void)
    {
        if (!*ltokens)
            return (ParaVMToken *)null;

        ParaVMToken *tok = *ltokens++;

        lline = tok->line;
        lcolumn = tok->column;

        return tok;
    };

#define NEXT_TOKEN(VAR, TYPE) \
    ParaVMToken *VAR = next_token(); \
    \
    if (!VAR || VAR->type != PARAVM_TOKEN_TYPE_ ## TYPE) \
    { \
        result = PARAVM_ERROR_SYNTAX; \
        break; \
    }

    GHashTable *insns = g_hash_table_new_full(&g_direct_hash, &g_direct_equal, null, &free_hash_table);
    GHashTable *handlers = g_hash_table_new_full(&g_direct_hash, &g_direct_equal, null, &free_hash_table);

    const ParaVMFunction *func = null;
    const ParaVMBlock *block = null;
    bool have_regs = false;
    bool have_insns = false;

    ParaVMError result = PARAVM_ERROR_OK;
    ParaVMToken *t;

    while ((t = next_token()))
    {
        switch (t->type)
        {
            case PARAVM_TOKEN_TYPE_FUN:
            {
                NEXT_TOKEN(name, STRING);

                if (paravm_get_function(mod, name->value))
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                func = paravm_create_function(name->value);
                block = null;
                have_regs = false;

                paravm_add_function(mod, func);

                GHashTable *tab = g_hash_table_new_full(&g_direct_hash, &g_direct_equal, null, &free_ptr_array);
                g_hash_table_insert(insns, (ParaVMFunction *)func, tab);

                tab = g_hash_table_new(&g_direct_hash, &g_direct_equal);
                g_hash_table_insert(handlers, (ParaVMFunction *)func, tab);

                break;
            }
            case PARAVM_TOKEN_TYPE_ARG:
            {
                if (!func || have_regs || block)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                NEXT_TOKEN(name, STRING);

                if (paravm_get_argument(func, name->value))
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                paravm_add_register(func, paravm_create_register(name->value, true));

                break;
            }
            case PARAVM_TOKEN_TYPE_REG:
            {
                if (!func || block)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                NEXT_TOKEN(name, STRING);

                if (paravm_get_register(func, name->value))
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                have_regs = true;

                paravm_add_register(func, paravm_create_register(name->value, false));

                break;
            }
            case PARAVM_TOKEN_TYPE_BLK:
            {
                if (!func)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                NEXT_TOKEN(name, STRING);

                if (paravm_get_block(func, name->value))
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                block = paravm_create_block(name->value);
                have_insns = false;

                paravm_add_block(func, block);

                GHashTable *tab = g_hash_table_lookup(insns, func);
                GPtrArray *arr = g_ptr_array_new_with_free_func(&free_instruction);
                g_hash_table_insert(tab, (ParaVMBlock *)block, arr);

                break;
            }
            case PARAVM_TOKEN_TYPE_UNW:
            {
                if (!block || have_insns)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                NEXT_TOKEN(name, STRING);
                NEXT_TOKEN(reg_name, STRING);

                const ParaVMRegister *reg = paravm_get_register(func, reg_name->value);

                if (!reg)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                if (paravm_set_exception_register(block, reg) != PARAVM_ERROR_OK)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                GHashTable *tab = g_hash_table_lookup(handlers, func);
                g_hash_table_insert(tab, (ParaVMBlock *)block, name->value);

                break;
            }
            case PARAVM_TOKEN_TYPE_OPCODE:
            {
                if (!block)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                const ParaVMOpCode *opc = paravm_get_opcode_by_name(t->value);
                have_regs = true;

#define NEXT_REG(NUM) \
    const ParaVMRegister *reg ## NUM = null; \
    \
    if (opc->registers >= NUM) \
    { \
        NEXT_TOKEN(reg ## NUM ## _tok, STRING); \
        reg ## NUM = paravm_get_register(func, reg ## NUM ## _tok->value); \
        \
        if (!reg ## NUM) \
        { \
            result = PARAVM_ERROR_ASSEMBLY; \
            break; \
        } \
    }

                NEXT_REG(1);
                NEXT_REG(2);
                NEXT_REG(3);

                ParaVMOperand operand;

                if (opc->operand != PARAVM_OPERAND_TYPE_NONE)
                {
                    NEXT_TOKEN(lpar, PAREN_OPEN);

                    switch (opc->operand)
                    {
                        case PARAVM_OPERAND_TYPE_INTEGER:
                        {
                            NEXT_TOKEN(oper, INTEGER);
                            operand.string = oper->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_FLOAT:
                        {
                            NEXT_TOKEN(oper, FLOAT);
                            operand.string = oper->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_ATOM:
                        {
                            NEXT_TOKEN(oper, ATOM);
                            operand.string = oper->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_BITS:
                        {
                            NEXT_TOKEN(oper, BITS);
                            operand.string = oper->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_BLOCK:
                        {
                            NEXT_TOKEN(oper, STRING);
                            operand.string = oper->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_BLOCKS:
                        {
                            NEXT_TOKEN(oper1, STRING);
                            operand.blocks[0] = (const ParaVMBlock *)oper1->value;

                            NEXT_TOKEN(oper2, STRING);
                            operand.blocks[1] = (const ParaVMBlock *)oper2->value;

                            break;
                        }
                        case PARAVM_OPERAND_TYPE_ARGS:
                        {
                            GPtrArray *arg_arr = g_ptr_array_new();
                            ParaVMToken *t2;

                            while ((t2 = peek_token()) && t2->type != PARAVM_TOKEN_TYPE_PAREN_CLOSE)
                            {
                                NEXT_TOKEN(arg, STRING);
                                g_ptr_array_add(arg_arr, arg);
                            }

                            g_ptr_array_add(arg_arr, null);

                            operand.args = (const char *const *)arg_arr;

                            break;
                        }
                        default:
                            assert_unreachable();
                            break;
                    }

                    NEXT_TOKEN(rpar, PAREN_CLOSE);
                }

                const ParaVMInstruction *insn = paravm_create_instruction(opc, operand, false, reg1, reg2, reg3);

                GHashTable *tab = g_hash_table_lookup(insns, func);
                GPtrArray *arr = g_hash_table_lookup(tab, block);
                g_ptr_array_add(arr, (ParaVMInstruction *)insn);

                break;
            }
            default:
                result = PARAVM_ERROR_SYNTAX;
                break;
        }

        if (result != PARAVM_ERROR_OK)
        {
            // Try to give the actual location of the error.
            *line = t->line;
            *column = t->column;

            break;
        }
    }

    if (result == PARAVM_ERROR_OK)
    {
        GHashTableIter func_iter;
        g_hash_table_iter_init(&func_iter, insns);

        const ParaVMFunction *key_func;
        GHashTable *value_blocks;

        while (g_hash_table_iter_next(&func_iter, (gpointer *)&key_func, (gpointer *)&value_blocks))
        {
            GHashTableIter block_iter;
            g_hash_table_iter_init(&block_iter, value_blocks);

            const ParaVMBlock *key_block;
            GPtrArray *value_insns;

            while (g_hash_table_iter_next(&block_iter, (gpointer *)&key_block, (gpointer *)&value_insns))
            {
                for (uint32_t i = 0; i < value_insns->len; i++)
                {
                    const ParaVMInstruction *orig_insn = g_ptr_array_index(value_insns, i);

                    ParaVMOperand oper;
                    bool own = false;

                    switch (orig_insn->opcode->operand)
                    {
                        case PARAVM_OPERAND_TYPE_NONE:
                        case PARAVM_OPERAND_TYPE_INTEGER:
                        case PARAVM_OPERAND_TYPE_FLOAT:
                        case PARAVM_OPERAND_TYPE_ATOM:
                        case PARAVM_OPERAND_TYPE_BITS:
                        case PARAVM_OPERAND_TYPE_ARGS:
                            oper.raw = orig_insn->operand.raw;
                            own = true;

                            break;
                        case PARAVM_OPERAND_TYPE_BLOCK:
                            oper.block = paravm_get_block(key_func, orig_insn->operand.string);

                            if (!oper.block)
                                result = PARAVM_ERROR_ASSEMBLY;

                            break;
                        case PARAVM_OPERAND_TYPE_BLOCKS:
                            oper.blocks[0] = paravm_get_block(key_func, (const char *)orig_insn->operand.blocks[0]);
                            oper.blocks[1] = paravm_get_block(key_func, (const char *)orig_insn->operand.blocks[1]);

                            if (!oper.blocks[0] || !oper.blocks[1])
                                result = PARAVM_ERROR_ASSEMBLY;

                            break;
                        default:
                            assert_unreachable();
                            break;
                    }

                    if (result != PARAVM_ERROR_OK)
                        break;

                    const ParaVMInstruction *insn = paravm_create_instruction(orig_insn->opcode,
                                                                              oper,
                                                                              own,
                                                                              orig_insn->register1,
                                                                              orig_insn->register2,
                                                                              orig_insn->register3);

                    paravm_append_instruction(key_block, insn);
                }

                if (result != PARAVM_ERROR_OK)
                    break;
            }

            if (result != PARAVM_ERROR_OK)
                break;
        }

        g_hash_table_iter_init(&func_iter, handlers);

        while (g_hash_table_iter_next(&func_iter, (gpointer *)&key_func, (gpointer *)&value_blocks))
        {
            GHashTableIter block_iter;
            g_hash_table_iter_init(&block_iter, value_blocks);

            const ParaVMBlock *key_block;
            const char *value_str;

            while (g_hash_table_iter_next(&block_iter, (gpointer *)&key_block, (gpointer *)&value_str))
            {
                const ParaVMBlock *handler = paravm_get_block(key_func, value_str);

                if (!handler)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }

                if (paravm_set_handler_block(key_block, handler) != PARAVM_ERROR_OK)
                {
                    result = PARAVM_ERROR_ASSEMBLY;
                    break;
                }
            }

            if (result != PARAVM_ERROR_OK)
                break;
        }

        if (result == PARAVM_ERROR_OK)
        {
            *line = lline;
            *column = lcolumn;
        }
    }

    g_hash_table_destroy(insns);
    g_hash_table_destroy(handlers);

    return result;
}
