#include <glib.h>

#include "opcodes.h"

const ParaVMOpCode paravm_op_call = { "call", 0x00, 3, PARAVM_OPERAND_TYPE_ARGS };
const ParaVMOpCode paravm_op_ret = { "ret", 0x01, 1, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_ldi = { "ldi", 0x02, 1, PARAVM_OPERAND_TYPE_INTEGER };
const ParaVMOpCode paravm_op_ldf = { "ldf", 0x03, 1, PARAVM_OPERAND_TYPE_FLOAT };
const ParaVMOpCode paravm_op_lda = { "lda", 0x04, 1, PARAVM_OPERAND_TYPE_ATOM };
const ParaVMOpCode paravm_op_lds = { "lds", 0x05, 1, PARAVM_OPERAND_TYPE_STRING };
const ParaVMOpCode paravm_op_ldb = { "ldb", 0x06, 1, PARAVM_OPERAND_TYPE_BITS };
const ParaVMOpCode paravm_op_mov = { "mov", 0x07, 2, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_add = { "add", 0x08, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_sub = { "sub", 0x09, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_mul = { "mul", 0x0A, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_div = { "div", 0x0B, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_rem = { "rem", 0x0C, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_neg = { "neg", 0x0D, 2, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_and = { "and", 0x0E, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_or = { "or", 0x0F, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_xor = { "xor", 0x10, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_not = { "not", 0x11, 2, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_shl = { "shl", 0x12, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_shr = { "shr", 0x13, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_stg = { "stg", 0x14, 1, PARAVM_OPERAND_TYPE_GLOBAL };
const ParaVMOpCode paravm_op_ldg = { "ldg", 0x15, 1, PARAVM_OPERAND_TYPE_GLOBAL };
const ParaVMOpCode paravm_op_jmp = { "jmp", 0x16, 0, PARAVM_OPERAND_TYPE_BLOCK };
const ParaVMOpCode paravm_op_jmpc = { "jmpc", 0x17, 1, PARAVM_OPERAND_TYPE_BLOCK };
const ParaVMOpCode paravm_op_clt = { "clt", 0x18, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_cgt = { "cgt", 0x19, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_cne = { "cne", 0x1A, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_ceq = { "ceq", 0x1B, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_cge = { "cge", 0x1C, 3, PARAVM_OPERAND_TYPE_NONE };
const ParaVMOpCode paravm_op_cle = { "cle", 0x1D, 3, PARAVM_OPERAND_TYPE_NONE };

static const ParaVMOpCode *opcodes[] =
{
    &paravm_op_call,
    &paravm_op_ret,
    &paravm_op_ldi,
    &paravm_op_ldf,
    &paravm_op_lda,
    &paravm_op_lds,
    &paravm_op_ldb,
    &paravm_op_mov,
    &paravm_op_add,
    &paravm_op_sub,
    &paravm_op_mul,
    &paravm_op_div,
    &paravm_op_rem,
    &paravm_op_neg,
    &paravm_op_and,
    &paravm_op_or,
    &paravm_op_xor,
    &paravm_op_not,
    &paravm_op_shl,
    &paravm_op_shr,
    &paravm_op_stg,
    &paravm_op_ldg,
    &paravm_op_jmp,
    &paravm_op_jmpc,
    &paravm_op_clt,
    &paravm_op_cgt,
    &paravm_op_cne,
    &paravm_op_ceq,
    &paravm_op_cge,
    &paravm_op_cle,
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

const ParaVMOpCode *paravm_get_opcodes(void)
{
    return opcodes[0];
}

const ParaVMOpCode *paravm_get_opcode_by_name(const char *name)
{
    return g_hash_table_lookup(opcode_name_table, &name);
}

const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code)
{
    return g_hash_table_lookup(opcode_code_table, &code);
}
