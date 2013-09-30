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
};

/* Verifies that `mod` contains sensible code for execution.
 * A module that does not pass this check should never be
 * passed to the execution engine. `*offender_fun` and
 * `*offender_blk` are first set to `NULL`, after which the
 * verification proceeds. If an error occurred,
 * `*offender_fun` will point to the offending function,
 * while `*offender_blk` will point to the offending block.
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
                                          const ParaVMBlock **offender_blk);

paravm_end
