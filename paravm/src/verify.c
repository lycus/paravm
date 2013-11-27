#include <string.h>

#include "verify.h"

ParaVMVerifierResult paravm_verify_module(const ParaVMModule *mod,
                                          const ParaVMFunction **offender_fun,
                                          const ParaVMBlock **offender_blk,
                                          const ParaVMInstruction **offender_insn)
{
    assert(mod);
    assert(offender_fun);
    assert(offender_blk);
    assert(offender_insn);

    *offender_fun = null;
    *offender_blk = null;
    *offender_insn = null;

    for (const ParaVMFunction *const *f = paravm_get_functions(mod); *f; f++)
    {
        for (const ParaVMBlock *const *b = paravm_get_blocks(*f); *b; b++)
        {
            bool have_term = false;

            for (const ParaVMInstruction *const *i = paravm_get_instructions(*b); *i; i++)
            {
                const ParaVMOpCode *op = (*i)->opcode;

                if ((op == &paravm_op_bin_efs ||
                     op == &paravm_op_bin_efd ||
                     op == &paravm_op_bin_dfs ||
                     op == &paravm_op_bin_dfd ||
                     op == &paravm_op_bin_eisu ||
                     op == &paravm_op_bin_dis ||
                     op == &paravm_op_bin_diu) &&
                    strcmp ((*i)->operand.string, "little") &&
                    strcmp ((*i)->operand.string, "big") &&
                    strcmp ((*i)->operand.string, "native"))
                {
                    *offender_fun = *f;
                    *offender_blk = *b;
                    *offender_insn = *i;

                    return PARAVM_VERIFIER_BAD_ENDIANNESS;
                }

                if ((*i)->opcode->control_flow != PARAVM_CONTROL_FLOW_NONE)
                {
                    if (have_term)
                    {
                        *offender_fun = *f;
                        *offender_blk = *b;

                        return PARAVM_VERIFIER_MULTIPLE_TERMINATORS;
                    }

                    have_term = true;
                }
            }

            if (!have_term)
            {
                *offender_fun = *f;
                *offender_blk = *b;

                return PARAVM_VERIFIER_NO_TERMINATOR;
            }
        }
    }

    return PARAVM_VERIFIER_OK;
}
