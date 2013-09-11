#pragma once

#include "common.h"

typedef enum {
    PARAVM_OPERAND_TYPE_NONE = 0,
    PARAVM_OPERAND_TYPE_INTEGER = 1,
    PARAVM_OPERAND_TYPE_FLOAT = 2,
    PARAVM_OPERAND_TYPE_ATOM = 3,
    PARAVM_OPERAND_TYPE_STRING = 4,
    PARAVM_OPERAND_TYPE_BITS = 5,
    PARAVM_OPERAND_TYPE_ARGS = 6,
    PARAVM_OPERAND_TYPE_GLOBAL = 7,
    PARAVM_OPERAND_TYPE_BLOCK = 8,
} ParaVMOperandType;

typedef struct {
    const char *name;
    uint8_t code;
    uint8_t registers;
    ParaVMOperandType operand;
} ParaVMOpCode;

extern const ParaVMOpCode paravm_op_call;
extern const ParaVMOpCode paravm_op_ret;
extern const ParaVMOpCode paravm_op_ldi;
extern const ParaVMOpCode paravm_op_ldf;
extern const ParaVMOpCode paravm_op_lda;
extern const ParaVMOpCode paravm_op_lds;
extern const ParaVMOpCode paravm_op_ldb;
extern const ParaVMOpCode paravm_op_mov;
extern const ParaVMOpCode paravm_op_add;
extern const ParaVMOpCode paravm_op_sub;
extern const ParaVMOpCode paravm_op_mul;
extern const ParaVMOpCode paravm_op_div;
extern const ParaVMOpCode paravm_op_rem;
extern const ParaVMOpCode paravm_op_neg;
extern const ParaVMOpCode paravm_op_and;
extern const ParaVMOpCode paravm_op_or;
extern const ParaVMOpCode paravm_op_xor;
extern const ParaVMOpCode paravm_op_not;
extern const ParaVMOpCode paravm_op_shl;
extern const ParaVMOpCode paravm_op_shr;
extern const ParaVMOpCode paravm_op_stg;
extern const ParaVMOpCode paravm_op_ldg;
extern const ParaVMOpCode paravm_op_jmp;
extern const ParaVMOpCode paravm_op_jmpc;
extern const ParaVMOpCode paravm_op_clt;
extern const ParaVMOpCode paravm_op_cgt;
extern const ParaVMOpCode paravm_op_cne;
extern const ParaVMOpCode paravm_op_ceq;
extern const ParaVMOpCode paravm_op_cge;
extern const ParaVMOpCode paravm_op_cle;

paravm_api
paravm_nothrow
paravm_pure
paravm_nonnull()
const ParaVMOpCode *paravm_get_opcodes(void);

paravm_api
paravm_nothrow
paravm_pure
paravm_nonnull()
const ParaVMOpCode *paravm_get_opcode_by_name(const char *name);

paravm_api
paravm_nothrow
paravm_pure
const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code);
