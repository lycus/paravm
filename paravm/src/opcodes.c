#include <glib.h>

#include "opcodes.h"

const ParaVMOpCode paravm_op_call = { "call", 0x00, 3, PARAVM_OPERAND_TYPE_ARGS };
const ParaVMOpCode paravm_op_ret = { "ret", 0x01, 1, PARAVM_OPERAND_TYPE_NONE };

static const ParaVMOpCode *opcodes[] =
{
    &paravm_op_call,
    &paravm_op_ret,
    null,
};

static GHashTable *opcode_name_table;
static GHashTable *opcode_code_table;

global_ctor
static void global_opcodes_ctor(void)
{
    opcode_name_table = g_hash_table_new(&g_str_hash, &g_str_equal);
    opcode_code_table = g_hash_table_new(&g_int_hash, &g_int_equal);

    for (const ParaVMOpCode **opc = &opcodes[0]; *opc; opc++)
    {
        const char *name = (*opc)->name;
        uint8_t code = (*opc)->code;

        g_hash_table_insert(opcode_name_table, &name, opc);
        g_hash_table_insert(opcode_code_table, &code, opc);
    }
}

global_dtor
static void global_opcodes_dtor(void)
{
    g_hash_table_destroy(opcode_name_table);
    g_hash_table_destroy(opcode_code_table);
}

const ParaVMOpCode **paravm_get_opcodes(void)
{
    return opcodes;
}

const ParaVMOpCode *paravm_get_opcode_by_name(const char *name)
{
    return g_hash_table_lookup(opcode_name_table, &name);
}

const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code)
{
    return g_hash_table_lookup(opcode_code_table, &code);
}
