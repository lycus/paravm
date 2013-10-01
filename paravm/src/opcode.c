#include <glib.h>

#include "opcode.h"

const ParaVMOpCode paravm_op_app = { "app", 0x00, 3, PARAVM_OPERAND_TYPE_ARGS, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_ret = { "ret", 0x01, 1, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_RETURN };
const ParaVMOpCode paravm_op_ldi = { "ldi", 0x02, 1, PARAVM_OPERAND_TYPE_INTEGER, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_ldf = { "ldf", 0x03, 1, PARAVM_OPERAND_TYPE_FLOAT, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_lda = { "lda", 0x04, 1, PARAVM_OPERAND_TYPE_ATOM, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_ldb = { "ldb", 0x05, 1, PARAVM_OPERAND_TYPE_BINARY, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_mov = { "mov", 0x06, 2, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_add = { "add", 0x07, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_sub = { "sub", 0x08, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_mul = { "mul", 0x09, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_div = { "div", 0x0A, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_rem = { "rem", 0x0B, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_pow = { "pow", 0x0C, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_neg = { "neg", 0x0D, 2, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_and = { "and", 0x0E, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_or = { "or", 0x0F, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_xor = { "xor", 0x10, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_not = { "not", 0x11, 2, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_shl = { "shl", 0x12, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_shr = { "shr", 0x13, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_br = { "br", 0x14, 0, PARAVM_OPERAND_TYPE_BLOCK, PARAVM_CONTROL_FLOW_BRANCH };
const ParaVMOpCode paravm_op_brc = { "brc", 0x15, 1, PARAVM_OPERAND_TYPE_BLOCKS, PARAVM_CONTROL_FLOW_BRANCH };
const ParaVMOpCode paravm_op_clt = { "clt", 0x16, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_cgt = { "cgt", 0x17, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_cne = { "cne", 0x18, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_ceq = { "ceq", 0x19, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_cge = { "cge", 0x1A, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_cle = { "cle", 0x1B, 3, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_upc = { "upc", 0x1C, 3, PARAVM_OPERAND_TYPE_ARGS, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_eht = { "eht", 0x1D, 1, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_THROW };
const ParaVMOpCode paravm_op_ehc = { "ehc", 0x1E, 1, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };
const ParaVMOpCode paravm_op_ehr = { "ehr", 0x1F, 0, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_THROW };
const ParaVMOpCode paravm_op_tcr = { "tcr", 0x20, 2, PARAVM_OPERAND_TYPE_ARGS, PARAVM_CONTROL_FLOW_RETURN };
const ParaVMOpCode paravm_op_nop = { "nop", 0x21, 0, PARAVM_OPERAND_TYPE_NONE, PARAVM_CONTROL_FLOW_NONE };

static const ParaVMOpCode *opcodes[] =
{
    &paravm_op_app,
    &paravm_op_ret,
    &paravm_op_ldi,
    &paravm_op_ldf,
    &paravm_op_lda,
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
    &paravm_op_br,
    &paravm_op_brc,
    &paravm_op_clt,
    &paravm_op_cgt,
    &paravm_op_cne,
    &paravm_op_ceq,
    &paravm_op_cge,
    &paravm_op_cle,
    &paravm_op_upc,
    &paravm_op_eht,
    &paravm_op_ehc,
    &paravm_op_ehr,
    &paravm_op_tcr,
    &paravm_op_nop,
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
