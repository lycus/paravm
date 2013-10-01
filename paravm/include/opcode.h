#pragma once

#include "common.h"

paravm_begin

typedef enum ParaVMOperandType ParaVMOperandType;

/* Specifies the kind of operand that an opcode expects.
 */
enum ParaVMOperandType
{
    PARAVM_OPERAND_TYPE_NONE = 0, // No operand.
    PARAVM_OPERAND_TYPE_INTEGER = 1, // An integer literal.
    PARAVM_OPERAND_TYPE_FLOAT = 2, // A floating point literal.
    PARAVM_OPERAND_TYPE_ATOM = 3, // An atom literal.
    PARAVM_OPERAND_TYPE_BINARY = 4, // A binary literal.
    PARAVM_OPERAND_TYPE_ARGS = 5, // An argument list.
    PARAVM_OPERAND_TYPE_BLOCK = 6, // A basic block.
    PARAVM_OPERAND_TYPE_BLOCKS = 7, // Two basic blocks.
};

typedef enum ParaVMControlFlow ParaVMControlFlow;

/* Specifies the control flow effects of an opcode.
 */
enum ParaVMControlFlow
{
    PARAVM_CONTROL_FLOW_NONE = 0, // No control flow.
    PARAVM_CONTROL_FLOW_BRANCH = 1, // Branches to another basic block.
    PARAVM_CONTROL_FLOW_RETURN = 2, // Exits the current function.
    PARAVM_CONTROL_FLOW_THROW = 3, // Throws an exception.
};

typedef struct ParaVMOpCode ParaVMOpCode;

/* Defines the structure of an opcode.
 */
struct ParaVMOpCode
{
    const char *name; // The name of the opcode.
    uint8_t code; // The byte code of the opcode.
    uint8_t registers; // The number of registers the opcode operates on.
    ParaVMOperandType operand; // The kind of operand the opcode expects.
    ParaVMControlFlow control_flow; // The control flow effect of the opcode.
};

extern const ParaVMOpCode paravm_op_app; // The `app` opcode.
extern const ParaVMOpCode paravm_op_ret; // The `ret` opcode.
extern const ParaVMOpCode paravm_op_ldi; // The `ldi` opcode.
extern const ParaVMOpCode paravm_op_ldf; // The `ldf` opcode.
extern const ParaVMOpCode paravm_op_lda; // The `lda` opcode.
extern const ParaVMOpCode paravm_op_ldb; // The `ldb` opcode.
extern const ParaVMOpCode paravm_op_mov; // The `mov` opcode.
extern const ParaVMOpCode paravm_op_add; // The `add` opcode.
extern const ParaVMOpCode paravm_op_sub; // The `sub` opcode.
extern const ParaVMOpCode paravm_op_mul; // The `mul` opcode.
extern const ParaVMOpCode paravm_op_div; // The `div` opcode.
extern const ParaVMOpCode paravm_op_rem; // The `rem` opcode.
extern const ParaVMOpCode paravm_op_pow; // The `pow` opcode.
extern const ParaVMOpCode paravm_op_neg; // The `neg` opcode.
extern const ParaVMOpCode paravm_op_and; // The `and` opcode.
extern const ParaVMOpCode paravm_op_or; // The `or` opcode.
extern const ParaVMOpCode paravm_op_xor; // The `xor` opcode.
extern const ParaVMOpCode paravm_op_not; // The `not` opcode.
extern const ParaVMOpCode paravm_op_shl; // The `shl` opcode.
extern const ParaVMOpCode paravm_op_shr; // The `shr` opcode.
extern const ParaVMOpCode paravm_op_br; // The `br` opcode.
extern const ParaVMOpCode paravm_op_brc; // The `brc` opcode.
extern const ParaVMOpCode paravm_op_clt; // The `clt` opcode.
extern const ParaVMOpCode paravm_op_cgt; // The `cgt` opcode.
extern const ParaVMOpCode paravm_op_cne; // The `cne` opcode.
extern const ParaVMOpCode paravm_op_ceq; // The `ceq` opcode.
extern const ParaVMOpCode paravm_op_cge; // The `cge` opcode.
extern const ParaVMOpCode paravm_op_cle; // The `cle` opcode.
extern const ParaVMOpCode paravm_op_upc; // The `upc` opcode.
extern const ParaVMOpCode paravm_op_eht; // The `eht` opcode.
extern const ParaVMOpCode paravm_op_ehc; // The `ehc` opcode.
extern const ParaVMOpCode paravm_op_ehr; // The `ehr` opcode.
extern const ParaVMOpCode paravm_op_tcr; // The `tcr` opcode.
extern const ParaVMOpCode paravm_op_nop; // The `nop` opcode.

/* Gets a list of pointers to all opcodes. The list is
 * `NULL`-terminated.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMOpCode *const *paravm_get_opcodes(void);

/* Gets an opcode with a name equal to `name`.
 *
 * Returns the matching opcode or `NULL`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMOpCode *paravm_get_opcode_by_name(const char *name);

/* Gets an opcode with a byte code equal to `code`.
 *
 * Returns the matching opcode or `NULL`.
 */
paravm_api
paravm_nothrow
const ParaVMOpCode *paravm_get_opcode_by_code(uint8_t code);

paravm_end
