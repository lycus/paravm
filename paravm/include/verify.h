#pragma once

#include "ir.h"

paravm_begin

typedef enum ParaVMVerifierResult ParaVMVerifierResult;

/* Indicates the result of a module verification operation.
 */
enum ParaVMVerifierResult
{
    PARAVM_VERIFIER_OK = 0, // The module was verified with no problems.
    PARAVM_VERIFIER_NO_TERMINATOR = 1, // A basic block did not have a terminator.
    PARAVM_VERIFIER_MULTIPLE_TERMINATORS = 2, // A basic block contained multiple terminators.
    PARAVM_VERIFIER_BAD_ENDIANNESS = 3, // An endianness atom was not `'little'`, `'big'` or `'native'`.
};

/* Verifies that `mod` contains sensible code for execution.
 * A module that does not pass this check should never be
 * passed to the execution engine. `*offender_fun`,
 * `*offender_blk`, and `*offender_insn` are first set to
 * `NULL`, after which the verification proceeds. If an
 * error occurred, `*offender_fun` will point to the
 * offending function, while `*offender_blk` will point to
 * the offending block. `*offender_insn` will point to the
 * offending instruction if one is relevant to the error;
 * otherwise, it will be `NULL`.
 *
 * This function returns as soon as any error is found, so
 * after fixing an error that this function reports, it
 * should be executed again to see if a module is error-free.
 *
 * Note that this function assumes that instructions are
 * well-formed; that is, if an instruction takes an atom
 * operand, this function expects the instruction's operand
 * to actually be an atom, and so on. It is assumed that
 * whatever code loaded a module into memory made sure that
 * such invariants are upheld.
 *
 * Returns one of the `ParaVMVerifierResult` values to
 * indicate the result of the verification operation. If the
 * returned value is `PARAVM_VERIFIER_OK`, the module is
 * correct and can safely be executed.
 */
paravm_api
paravm_nothrow
paravm_nonnull()
ParaVMVerifierResult paravm_verify_module(const ParaVMModule *mod,
                                          const ParaVMFunction **offender_fun,
                                          const ParaVMBlock **offender_blk,
                                          const ParaVMInstruction **offender_insn);

paravm_end
