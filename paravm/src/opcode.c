#include <glib.h>

#include "opcode.h"

#define OPCODE1(name, regs, var_regs, oper_type, cf_type) \
    const ParaVMOpCode paravm_op_ ## name = \
    { \
        STRINGIFY(name), \
        __COUNTER__, \
        regs, \
        var_regs, \
        PARAVM_OPERAND_TYPE_ ## oper_type, \
        PARAVM_CONTROL_FLOW_ ## cf_type \
    }

#define OPCODE2(cat, name, regs, var_regs, oper_type, cf_type) \
    const ParaVMOpCode paravm_op_ ## cat ## _ ## name = \
    { \
        STRINGIFY(cat) "." STRINGIFY(name), \
        __COUNTER__, \
        regs, \
        var_regs, \
        PARAVM_OPERAND_TYPE_ ## oper_type, \
        PARAVM_CONTROL_FLOW_ ## cf_type \
    }

OPCODE1(noop, 0, false, NONE, NONE);
OPCODE1(copy, 2, false, NONE, NONE);
OPCODE1(type, 2, false, NONE, NONE);
OPCODE2(load, nil, 1, false, NONE, NONE);
OPCODE2(load, int, 1, false, INTEGER, NONE);
OPCODE2(load, flt, 1, false, FLOAT, NONE);
OPCODE2(load, atom, 1, false, ATOM, NONE);
OPCODE2(load, bin, 1, false, BINARY, NONE);
OPCODE2(load, func, 3, true, NONE, NONE);
OPCODE2(num, add, 3, false, NONE, NONE);
OPCODE2(num, sub, 3, false, NONE, NONE);
OPCODE2(num, mul, 3, false, NONE, NONE);
OPCODE2(num, div, 3, false, NONE, NONE);
OPCODE2(num, rem, 3, false, NONE, NONE);
OPCODE2(num, pow, 3, false, NONE, NONE);
OPCODE2(num, neg, 2, false, NONE, NONE);
OPCODE2(num, and, 3, false, NONE, NONE);
OPCODE2(num, or, 3, false, NONE, NONE);
OPCODE2(num, xor, 3, false, NONE, NONE);
OPCODE2(num, not, 2, false, NONE, NONE);
OPCODE2(num, shl, 3, false, NONE, NONE);
OPCODE2(num, shr, 3, false, NONE, NONE);
OPCODE2(cmp, lt, 3, false, NONE, NONE);
OPCODE2(cmp, gt, 3, false, NONE, NONE);
OPCODE2(cmp, eq, 3, false, NONE, NONE);
OPCODE2(cmp, neq, 3, false, NONE, NONE);
OPCODE2(cmp, lteq, 3, false, NONE, NONE);
OPCODE2(cmp, gteq, 3, false, NONE, NONE);
OPCODE2(call, rem, 3, true, NONE, NONE);
OPCODE2(call, func, 2, true, NONE, NONE);
OPCODE2(call, up, 3, true, NONE, NONE);
OPCODE2(tup, make, 1, true, NONE, NONE);
OPCODE2(tup, get, 3, false, NONE, NONE);
OPCODE2(tup, set, 4, false, NONE, NONE);
OPCODE2(tup, del, 3, false, NONE, NONE);
OPCODE2(tup, size, 2, false, NONE, NONE);
OPCODE2(list, make, 1, true, NONE, NONE);
OPCODE2(list, head, 2, false, NONE, NONE);
OPCODE2(list, tail, 2, false, NONE, NONE);
OPCODE2(list, cons, 3, false, NONE, NONE);
OPCODE2(map, make, 1, true, NONE, NONE);
OPCODE2(map, add, 4, false, NONE, NONE);
OPCODE2(map, get, 3, false, NONE, NONE);
OPCODE2(map, del, 3, false, NONE, NONE);
OPCODE2(map, size, 2, false, NONE, NONE);
OPCODE2(map, keys, 3, false, NONE, NONE);
OPCODE2(map, vals, 3, false, NONE, NONE);
OPCODE2(set, make, 1, true, NONE, NONE);
OPCODE2(set, add, 3, false, NONE, NONE);
OPCODE2(set, find, 3, false, NONE, NONE);
OPCODE2(set, del, 3, false, NONE, NONE);
OPCODE2(set, size, 3, false, NONE, NONE);
OPCODE2(set, vals, 3, false, NONE, NONE);
OPCODE2(bin, size, 2, false, NONE, NONE);
OPCODE2(bin, ebin, 4, false, NONE, NONE);
OPCODE2(bin, dbin, 4, false, NONE, NONE);
OPCODE2(bin, efs, 4, false, ATOM, NONE);
OPCODE2(bin, efd, 4, false, ATOM, NONE);
OPCODE2(bin, dfs, 3, false, ATOM, NONE);
OPCODE2(bin, dfd, 3, false, ATOM, NONE);
OPCODE2(bin, eisu, 5, false, ATOM, NONE);
OPCODE2(bin, dis, 4, false, ATOM, NONE);
OPCODE2(bin, diu, 4, false, ATOM, NONE);
OPCODE2(jump, goto, 0, false, BLOCK, BRANCH);
OPCODE2(jump, cond, 1, false, BLOCKS, BRANCH);
OPCODE2(jump, ret, 1, false, NONE, RETURN);
OPCODE2(exc, new, 1, false, NONE, THROW);
OPCODE2(exc, get, 1, false, NONE, NONE);
OPCODE2(exc, cont, 0, false, NONE, THROW);

static const ParaVMOpCode *opcodes[] =
{
    &paravm_op_noop,
    &paravm_op_copy,
    &paravm_op_load_nil,
    &paravm_op_load_int,
    &paravm_op_load_flt,
    &paravm_op_load_atom,
    &paravm_op_load_bin,
    &paravm_op_load_func,
    &paravm_op_num_add,
    &paravm_op_num_sub,
    &paravm_op_num_mul,
    &paravm_op_num_div,
    &paravm_op_num_rem,
    &paravm_op_num_pow,
    &paravm_op_num_neg,
    &paravm_op_num_and,
    &paravm_op_num_or,
    &paravm_op_num_xor,
    &paravm_op_num_not,
    &paravm_op_num_shl,
    &paravm_op_num_shr,
    &paravm_op_cmp_lt,
    &paravm_op_cmp_gt,
    &paravm_op_cmp_eq,
    &paravm_op_cmp_neq,
    &paravm_op_cmp_lteq,
    &paravm_op_cmp_gteq,
    &paravm_op_call_func,
    &paravm_op_call_rem,
    &paravm_op_call_up,
    &paravm_op_tup_make,
    &paravm_op_tup_get,
    &paravm_op_tup_set,
    &paravm_op_tup_del,
    &paravm_op_tup_size,
    &paravm_op_list_make,
    &paravm_op_list_head,
    &paravm_op_list_tail,
    &paravm_op_list_cons,
    &paravm_op_map_make,
    &paravm_op_map_add,
    &paravm_op_map_del,
    &paravm_op_map_get,
    &paravm_op_map_size,
    &paravm_op_map_keys,
    &paravm_op_map_vals,
    &paravm_op_set_make,
    &paravm_op_set_add,
    &paravm_op_set_del,
    &paravm_op_set_find,
    &paravm_op_set_size,
    &paravm_op_set_vals,
    &paravm_op_bin_size,
    &paravm_op_bin_ebin,
    &paravm_op_bin_dbin,
    &paravm_op_bin_efs,
    &paravm_op_bin_efd,
    &paravm_op_bin_dfs,
    &paravm_op_bin_dfd,
    &paravm_op_bin_eisu,
    &paravm_op_bin_dis,
    &paravm_op_bin_diu,
    &paravm_op_jump_goto,
    &paravm_op_jump_cond,
    &paravm_op_jump_ret,
    &paravm_op_exc_new,
    &paravm_op_exc_get,
    &paravm_op_exc_cont,
    null,
};

static GHashTable *opcode_name_table;
static GHashTable *opcode_code_table;

global_ctor
static void global_opcode_ctor(void)
{
    opcode_name_table = g_hash_table_new(&g_str_hash, &g_str_equal);
    opcode_code_table = g_hash_table_new_full(&g_int_hash, &g_int_equal, &g_free, null);

    for (const ParaVMOpCode **opc = &opcodes[0]; *opc; opc++)
    {
        g_hash_table_insert(opcode_name_table, (gpointer)(*opc)->name, (gpointer)*opc);

        uint32_t *p = g_new(uint32_t, 1);
        *p = (*opc)->code;

        g_hash_table_insert(opcode_code_table, p, (gpointer)*opc);
    }
}

global_dtor
static void global_opcode_dtor(void)
{
    g_hash_table_destroy(opcode_name_table);
    g_hash_table_destroy(opcode_code_table);
}

const ParaVMOpCode *const *paravm_get_opcodes(void)
{
    return opcodes;
}

const ParaVMOpCode *paravm_get_opcode_by_name(const char *name)
{
    assert(name);

    return g_hash_table_lookup(opcode_name_table, name);
}

const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code)
{
    int c = code;

    return g_hash_table_lookup(opcode_code_table, &c);
}
