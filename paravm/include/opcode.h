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
    PARAVM_OPERAND_TYPE_BLOCK = 5, // A basic block.
    PARAVM_OPERAND_TYPE_BLOCKS = 6, // Two basic blocks.
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
    uint8_t registers; // The number of registers the opcode requires.
    bool variable_registers; // Whether any number of additional registers can be given.
    ParaVMOperandType operand; // The kind of operand the opcode expects.
    ParaVMControlFlow control_flow; // The control flow effect of the opcode.
};

extern const ParaVMOpCode paravm_op_noop; // The `noop` opcode.
extern const ParaVMOpCode paravm_op_copy; // The `copy` opcode.
extern const ParaVMOpCode paravm_op_type; // The `type` opcode.
extern const ParaVMOpCode paravm_op_load_nil; // The `load.nil` opcode.
extern const ParaVMOpCode paravm_op_load_int; // The `load.int` opcode.
extern const ParaVMOpCode paravm_op_load_flt; // The `load.flt` opcode.
extern const ParaVMOpCode paravm_op_load_atom; // The `load.atom` opcode.
extern const ParaVMOpCode paravm_op_load_bin; // The `load.bin` opcode.
extern const ParaVMOpCode paravm_op_load_func; // The `load.func` opcode.
extern const ParaVMOpCode paravm_op_num_add; // The `num.add` opcode.
extern const ParaVMOpCode paravm_op_num_sub; // The `num.sub` opcode.
extern const ParaVMOpCode paravm_op_num_mul; // The `num.mul` opcode.
extern const ParaVMOpCode paravm_op_num_div; // The `num.div` opcode.
extern const ParaVMOpCode paravm_op_num_rem; // The `num.rem` opcode.
extern const ParaVMOpCode paravm_op_num_pow; // The `num.pow` opcode.
extern const ParaVMOpCode paravm_op_num_neg; // The `num.neg` opcode.
extern const ParaVMOpCode paravm_op_num_and; // The `num.and` opcode.
extern const ParaVMOpCode paravm_op_num_or; // The `num.or` opcode.
extern const ParaVMOpCode paravm_op_num_xor; // The `num.xor` opcode.
extern const ParaVMOpCode paravm_op_num_not; // The `num.not` opcode.
extern const ParaVMOpCode paravm_op_num_shl; // The `num.shl` opcode.
extern const ParaVMOpCode paravm_op_num_shr; // The `num.shr` opcode.
extern const ParaVMOpCode paravm_op_cmp_lt; // The `cmp.lt` opcode.
extern const ParaVMOpCode paravm_op_cmp_gt; // The `cmp.gt` opcode.
extern const ParaVMOpCode paravm_op_cmp_eq; // The `cmp.eq` opcode.
extern const ParaVMOpCode paravm_op_cmp_neq; // The `cmp.neq` opcode.
extern const ParaVMOpCode paravm_op_cmp_lteq; // The `cmp.lteq` opcode.
extern const ParaVMOpCode paravm_op_cmp_gteq; // The `cmp.gteq` opcode.
extern const ParaVMOpCode paravm_op_call_rem; // The `call.rem` opcode.
extern const ParaVMOpCode paravm_op_call_func; // The `call.func` opcode.
extern const ParaVMOpCode paravm_op_call_up; // The `call.up` opcode.
extern const ParaVMOpCode paravm_op_tup_make; // The `tup.make` opcode.
extern const ParaVMOpCode paravm_op_tup_get; // The `tup.get` opcode.
extern const ParaVMOpCode paravm_op_tup_set; // The `tup.set` opcode.
extern const ParaVMOpCode paravm_op_tup_del; // The `tup.set` opcode.
extern const ParaVMOpCode paravm_op_tup_size; // The `tup.size` opcode.
extern const ParaVMOpCode paravm_op_list_make; // The `list.make` opcode.
extern const ParaVMOpCode paravm_op_list_head; // The `list.head` opcode.
extern const ParaVMOpCode paravm_op_list_tail; // The `list.tail` opcode.
extern const ParaVMOpCode paravm_op_list_cons; // The `list.cons` opcode.
extern const ParaVMOpCode paravm_op_map_make; // The `map.make` opcode.
extern const ParaVMOpCode paravm_op_map_add; // The `map.add` opcode.
extern const ParaVMOpCode paravm_op_map_get; // The `map.get` opcode.
extern const ParaVMOpCode paravm_op_map_del; // The `map.del` opcode.
extern const ParaVMOpCode paravm_op_map_size; // The `map.size` opcode.
extern const ParaVMOpCode paravm_op_map_keys; // The `map.keys` opcode.
extern const ParaVMOpCode paravm_op_map_vals; // The `map.vals` opcode.
extern const ParaVMOpCode paravm_op_set_make; // The `set.make` opcode.
extern const ParaVMOpCode paravm_op_set_add; // The `set.add` opcode.
extern const ParaVMOpCode paravm_op_set_find; // The `set.find` opcode.
extern const ParaVMOpCode paravm_op_set_del; // The `set.del` opcode.
extern const ParaVMOpCode paravm_op_set_size; // The `set.size` opcode.
extern const ParaVMOpCode paravm_op_set_vals; // The `set.vals` opcode.
extern const ParaVMOpCode paravm_op_bin_size; // The `bin.size` opcode.
extern const ParaVMOpCode paravm_op_bin_ebin; // The `bin.ebin` opcode.
extern const ParaVMOpCode paravm_op_bin_dbin; // The `bin.dbin` opcode.
extern const ParaVMOpCode paravm_op_bin_efs; // The `bin.efs` opcode.
extern const ParaVMOpCode paravm_op_bin_efd; // The `bin.efd` opcode.
extern const ParaVMOpCode paravm_op_bin_dfs; // The `bin.dfs` opcode.
extern const ParaVMOpCode paravm_op_bin_dfd; // The `bin.dfd` opcode.
extern const ParaVMOpCode paravm_op_bin_eisu; // The `bin.eisu` opcode.
extern const ParaVMOpCode paravm_op_bin_dis; // The `bin.dis` opcode.
extern const ParaVMOpCode paravm_op_bin_diu; // The `bin.diu` opcode.
extern const ParaVMOpCode paravm_op_jump_goto; // The `jump.goto` opcode.
extern const ParaVMOpCode paravm_op_jump_cond; // The `jump.cond` opcode.
extern const ParaVMOpCode paravm_op_jump_ret; // The `jump.ret` opcode.
extern const ParaVMOpCode paravm_op_exc_new; // The `exc.new` opcode.
extern const ParaVMOpCode paravm_op_exc_get; // The `exc.get` opcode.
extern const ParaVMOpCode paravm_op_exc_cont; // The `exc.cont` opcode.

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
