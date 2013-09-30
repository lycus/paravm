#include "verify.h"

ParaVMVerifierResult paravm_verify_module(const ParaVMModule *mod,
                                          const ParaVMFunction **offender_fun,
                                          const ParaVMBlock **offender_blk)
{
    assert(mod);
    assert(offender_fun);
    assert(offender_blk);

    *offender_fun = null;
    *offender_blk = null;

    for (const ParaVMFunction *const *f = paravm_get_functions(mod); *f; f++)
    {
        for (const ParaVMBlock *const *b = paravm_get_blocks(*f); *b; b++)
        {
            bool have_term = false;

            for (const ParaVMInstruction *const *i = paravm_get_instructions(*b); *i; i++)
            {
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
