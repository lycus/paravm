#include <glib.h>

#include "context.h"

bool paravm_has_esdk(void)
{
#if defined(PARAVM_EMULATE)
    return false;
#else
    return true;
#endif
}

ParaVMContext *paravm_create_context(const char *hdf)
{
#if defined(PARAVM_EMULATE)
    if (hdf)
        return null;
#endif

    ParaVMContext *ctx = g_new(ParaVMContext, 1);

    // TODO: Read hdf and check for errors.
    ctx->hdf = hdf;
    ctx->atom_table = paravm_create_atom_table();

    return ctx;
}

void paravm_destroy_context(ParaVMContext *ctx)
{
    if (ctx)
        paravm_destroy_atom_table(ctx->atom_table);

    g_free(ctx);
}
