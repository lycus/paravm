#pragma once

#include "common.h"

typedef enum {
    PARAVM_OPERAND_TYPE_NONE = 0,
    PARAVM_OPERAND_TYPE_INTEGER = 1,
    PARAVM_OPERAND_TYPE_FLOAT = 2,
    PARAVM_OPERAND_TYPE_ATOM = 3,
    PARAVM_OPERAND_TYPE_STRING = 4,
    PARAVM_OPERAND_TYPE_BITS = 5,
    PARAVM_OPERAND_TYPE_BYTES = 6,
    PARAVM_OPERAND_TYPE_ARGS = 7,
} ParaVMOperandType;

typedef struct {
    const char *name;
    uint8_t code;
    uint8_t registers;
    ParaVMOperandType operand;
} ParaVMOpCode;

extern const ParaVMOpCode paravm_op_call;
extern const ParaVMOpCode paravm_op_ret;

paravm_api
paravm_nothrow
paravm_pure
paravm_nonnull()
const ParaVMOpCode **paravm_get_opcodes(void);

paravm_api
paravm_nothrow
paravm_pure
paravm_nonnull()
const ParaVMOpCode *paravm_get_opcode_by_name(const char *name);

paravm_api
paravm_nothrow
paravm_pure
const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code);
