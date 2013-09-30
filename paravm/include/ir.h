#pragma once

#include "error.h"
#include "opcode.h"

paravm_begin

typedef struct ParaVMRegister ParaVMRegister;

/* Describes a virtual register. Functions can theoretically
 * house an infinite number of these, and instructions can
 * use them to store intermediate values and result values.
 */
struct ParaVMRegister
{
    const char *name; // The name of the register.
    bool argument; // Is the register a function argument?
};

typedef struct ParaVMBlock ParaVMBlock;

/* Describes a basic block. A block contains a series of
 * non-branching instructions and is terminated with an
 * instruction that does branch (e.g. to another block or
 * out of the function).
 */
struct ParaVMBlock
{
    const char *name; // The name of the block.
    const ParaVMBlock *handler; // Block to transfer control to if an exception is raised.
    const ParaVMRegister *exception; // Register to assign exception to.

    const void *instruction_list; // Private. Do not use.
};

typedef union ParaVMOperand ParaVMOperand;

/* Describes the operand of an instruction. Accessing fields
 * of this union only makes sense if the opcode has an
 * operand type not equal to `PARAVM_OPERAND_TYPE_NONE`.
 */
union ParaVMOperand
{
    const void *raw; // Raw pointer to the operand. Should generally be avoided.
    const char *string; // Pointer to a string operand.
    const char *const *args; // Pointer to an argument list operand. /* FIXME */
    const ParaVMBlock *block; // Pointer to a basic block operand.
    const ParaVMBlock *blocks[2]; // Pointers to basic block operands.
};

typedef struct ParaVMInstruction ParaVMInstruction;

/* Describes an instruction. These operate on some particular
 * registers and sometimes take an operand that is used to
 * perform the operation.
 */
struct ParaVMInstruction
{
    const ParaVMOpCode *opcode; // The opcode the instruction executes.
    ParaVMOperand operand; // The operand of the instruction.
    bool own_operand; // Whether the operand's lifetime is managed by this instruction.
    const ParaVMRegister *register1; // The first register, if any.
    const ParaVMRegister *register2; // The second register, if any.
    const ParaVMRegister *register3; // The third register, if any.
};

typedef struct ParaVMFunction ParaVMFunction;

/* Describes a function. These consist of a set of arguments,
 * a set of virtual registers, and a set of basic blocks that
 * contain the instructions to be executed.
 *
 * In reality, an argument is just a virtual register with a
 * flag set that indicates that it's an incoming value.
 */
struct ParaVMFunction
{
    const char *name; // The name of the function.

    const void *argument_table; // Private. Do not use.
    const void *argument_list; // Private. Do not use.
    const void *register_table; // Private. Do not use.
    const void *register_list; // Private. Do not use.
    const void *block_table; // Private. Do not use.
    const void *block_list; // Private. Do not use.
};

typedef struct ParaVMModule ParaVMModule;

/* Describes a module. These are collections of functions and
 * mainly serve as versionable containers.
 */
struct ParaVMModule
{
    const char *name; // The name of the module, e.g. `foo` for `/bar/foo.pvc`.

    const void *function_table; // Private. Do not use.
    const void *function_list; // Private. Do not use.
};

/* Creates a new `ParaVMRegister` with the given values.
 * `name` is copied, so it needs to be freed by the caller.
 *
 * Returns a `ParaVMRegister` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMRegister *paravm_create_register(const char *name, bool argument);

/* Destroys `reg` if it is not `NULL`.
 */
paravm_api
paravm_nothrow
void paravm_destroy_register(const ParaVMRegister *reg);

/* Creates a new `ParaVMInstruction` with the given values.
 * `operand` will be copied if `own_operand` is `true`, in
 * which case the caller must free `operand` after calling
 * this function. Note that in the case where the `op` takes
 * a basic block (or blocks) as operand, `own_operand` is
 * ignored and set to `false`.
 *
 * In the `!own_operand` case, `operand` can technically
 * point to anything, even something that isn't a real
 * operand. This can be useful for intermediate construction
 * of instructions in memory. Note, however, that most of
 * the ParaVM API assumes it to be a valid operand.
 *
 * `operand` is expected to point to a `NULL`-terminated
 * array of pointers to strings if `op`'s operand type is
 * `PARAVM_OPERAND_TYPE_ARGS`.
 *
 * Returns a `ParaVMInstruction` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull(1)
const ParaVMInstruction *paravm_create_instruction(const ParaVMOpCode *op,
                                                   ParaVMOperand operand,
                                                   bool own_operand,
                                                   const ParaVMRegister *reg1,
                                                   const ParaVMRegister *reg2,
                                                   const ParaVMRegister *reg3);

/* Destroys `insn` if it is not `NULL`.
 */
paravm_api
paravm_nothrow
void paravm_destroy_instruction(const ParaVMInstruction *insn);

/* Creates a new `ParaVMBlock` with the given value. `name`
 * is copied, so it needs to be freed by the caller.
 *
 * Returns a `ParaVMBlock` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMBlock *paravm_create_block(const char *name);

/* Destroys `block` if it is not `NULL`.
 *
 * This also calls `paravm_destroy_instruction` on all
 * contained instructions.
 */
paravm_api
paravm_nothrow
void paravm_destroy_block(const ParaVMBlock *block);

/* Sets the handler block of `block` to `handler`.
 *
 * Returns `PARAVM_ERROR_ALREADY_SET` if the handler
 * block of `block` has already been set to a value
 * that is not `NULL`. Otherwise, `PARAVM_ERROR_OK`.
 */
paravm_api
paravm_nothrow
paravm_nonnull(1)
ParaVMError paravm_set_handler_block(const ParaVMBlock *block, const ParaVMBlock *handler);

/* Sets the exception register of `block` to `exception`.
 *
 * Returns `PARAVM_ERROR_ALREADY_SET` if the exception
 * register of `block` has already been set to a value
 * that is not `NULL`. Otherwise, `PARAVM_ERROR_OK`.
 */
paravm_api
paravm_nothrow
paravm_nonnull(1)
ParaVMError paravm_set_exception_register(const ParaVMBlock *block, const ParaVMRegister *exception);

/* Prepends `insn` to the instruction list of `block`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
void paravm_prepend_instruction(const ParaVMBlock *block, const ParaVMInstruction *insn);

/* Appends `insn` to the instruction list of `block`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
void paravm_append_instruction(const ParaVMBlock *block, const ParaVMInstruction *insn);

/* Gets the instruction at the given index of `block`'s
 * instruction list.
 *
 * Returns the located instruction or `NULL` if `idx` is out
 * of bounds.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMInstruction *paravm_get_instruction(const ParaVMBlock *block, size_t idx);

/* Gets the position of `insn` in `block`'s instruction list.
 *
 * Returns the position or `SIZE_MAX` if `insn` is not in `block`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_instruction_index(const ParaVMBlock *block, const ParaVMInstruction *insn);

/* Gets a `NULL`-terminated array of instructions in `block`.
 * The returned pointer points into `block`, so it is tied to
 * `block`'s lifetime and does not need to be freed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMInstruction *const *paravm_get_instructions(const ParaVMBlock *block);

/* Gets the number of instructions in `block`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_instruction_count(const ParaVMBlock *block);

/* Creates a new `ParaVMFunction` with the given value.
 * `name` will be copied, so it needs to be freed by the
 * caller.
 *
 * Returns a `ParaVMFunction` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMFunction *paravm_create_function(const char *name);

/* Destroys `func` if it is not `NULL`.
 *
 * This also calls `paravm_destroy_register` and
 * `paravm_destroy_block` on all contained registers and
 * basic blocks, respectively.
 */
paravm_api
paravm_nothrow
void paravm_destroy_function(const ParaVMFunction *func);

/* Adds `block` to the list of basic blocks in `func`.
 *
 * Returns `PARAVM_ERROR_NAME_EXISTS` if a block with a name
 * equal to `block->name` already exists in `func`. Returns
 * `PARAVM_ERROR_OK` otherwise.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_add_block(const ParaVMFunction *func, const ParaVMBlock *block);

/* Finds a block with a name equal to `name` in `func`.
 *
 * Returns the located block or `NULL`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMBlock *paravm_get_block(const ParaVMFunction *func, const char *name);

/* Gets a `NULL`-terminated array of basic blocks in `func`.
 * The returned pointer points into `func`, so it is tied to
 * `func`'s lifetime and does not need to be freed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMBlock *const *paravm_get_blocks(const ParaVMFunction *func);

/* Gets the number of basic blocks in `func`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_block_count(const ParaVMFunction *func);

/* Adds `reg` to the list of registers in `func`.
 *
 * Returns `PARAVM_ERROR_NAME_EXISTS` if a register with a
 * name equal to `reg->name` already exists in `func`.
 * Returns `PARAVM_ERROR_OK` otherwise.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_add_register(const ParaVMFunction *func, const ParaVMRegister *reg);

/* Finds a register with a name equal to `name` in `func`.
 *
 * Returns the located register or `NULL`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMRegister *paravm_get_register(const ParaVMFunction *func, const char *name);

/* Gets a `NULL`-terminated array of registers in `func`.
 * The returned pointer points into `func`, so it is tied to
 * `func`'s lifetime and does not need to be freed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMRegister *const *paravm_get_registers(const ParaVMFunction *func);

/* Gets the number of registers in `func`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_register_count(const ParaVMFunction *func);

/* Finds an argument with a name equal to `name` in `func`.
 *
 * Returns the located argument or `NULL`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMRegister *paravm_get_argument(const ParaVMFunction *func, const char *name);

/* Gets a `NULL`-terminated array of arguments in `func`.
 * The returned pointer points into `func`, so it is tied to
 * `func`'s lifetime and does not need to be freed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMRegister *const *paravm_get_arguments(const ParaVMFunction *func);

/* Gets the number of arguments in `func`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_argument_count(const ParaVMFunction *func);

/* Creates a new `ParaVMModule` with the given value. `name`
 * will be copied, so it needs to be freed by the caller.
 *
 * Returns a `ParaVMModule` instance.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMModule *paravm_create_module(const char *name);

/* Destroys `mod` if it is not `NULL`.
 *
 * This also calls `paravm_destroy_function` on all contained
 * functions.
 */
paravm_api
paravm_nothrow
void paravm_destroy_module(const ParaVMModule *mod);

/* Adds `func` to the list of functions in `mod`.
 *
 * Returns `PARAVM_ERROR_NAME_EXISTS` if a function with a
 * name equal to `func->name` already exists in `mod`.
 * Returns `PARAVM_ERROR_OK` otherwise.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMError paravm_add_function(const ParaVMModule *mod, const ParaVMFunction *func);

/* Finds a block with a name equal to `name` in `func`.
 *
 * Returns the located block or `NULL`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMFunction *paravm_get_function(const ParaVMModule *mod, const char *name);

/* Gets a `NULL`-terminated array of functions in `mod`. The
 * returned pointer points into `mod`, so it is tied to
 * `mod`'s lifetime and does not need to be freed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
const ParaVMFunction *const *paravm_get_functions(const ParaVMModule *mod);

/* Gets the number of functions in `mod`.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
size_t paravm_get_function_count(const ParaVMModule *mod);

paravm_end
