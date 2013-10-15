#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "disassemble.h"

static ParaVMError errno_to_error(int err)
{
    assert(err);

    switch (err)
    {
        case EROFS:
            return PARAVM_ERROR_READ_ONLY;
        case EACCES:
            return PARAVM_ERROR_ACCESS;
        case ETXTBSY:
            return PARAVM_ERROR_BUSY;
        case EINTR:
            return PARAVM_ERROR_IO_INTERRUPT;
        case EISDIR:
            return PARAVM_ERROR_DIRECTORY;
        case ELOOP:
        case EMFILE:
        case ENFILE:
        case ENOMEM:
        case EFBIG:
            return PARAVM_ERROR_LIMIT;
        case EOVERFLOW:
            return PARAVM_ERROR_EOF;
        case ENAMETOOLONG:
            return PARAVM_ERROR_PATH_LENGTH;
        case ENOENT:
            return PARAVM_ERROR_NONEXISTENT;
        case ENOSPC:
            return PARAVM_ERROR_NO_SPACE;
        case ENOTDIR:
            return PARAVM_ERROR_NOT_DIRECTORY;
        case EIO:
            return PARAVM_ERROR_IO;
        default:
            assert_unreachable();
            return PARAVM_ERROR_OK;
    }
}

static void write(jmp_buf *sjlj, FILE *f, const char *value)
{
    assert(sjlj);
    assert(f);
    assert(value);

    if (!fwrite(value, strlen(value), 1, f))
        longjmp(*sjlj, 1);
}

static void write_str(jmp_buf *sjlj, FILE *f, const gchar *exc, const char *del, const char *value)
{
    assert(sjlj);
    assert(f);
    assert(del);
    assert(value);

    write(sjlj, f, del);

    gchar *esc = g_strescape(value, exc);

    // Do this manually so we can clean up.
    if (!fwrite(esc, strlen(esc), 1, f))
    {
        g_free(esc);
        longjmp(*sjlj, 1);
    }

    g_free(esc);

    write(sjlj, f, del);
}

ParaVMError paravm_disassemble_module(const ParaVMModule *mod, const char *path)
{
    assert(mod);
    assert(path);

    FILE *f = fopen(path, "w");

    if (!f)
        return errno_to_error(errno);

    jmp_buf sjlj;

    if (setjmp(sjlj))
    {
        int err = errno;

        fclose(f);
        return errno_to_error(err);
    }

    // Make all characters except `\`, `'`, and `"` exceptions.
    gchar *exc = g_new(gchar, 252 + 1);
    guchar c = 1;
    uint8_t i = 0;

    while (i < 252)
    {
        if (c != '\\' && c != '\'' && c != '"')
            exc[i++] = (gchar)c;

        c++;
    }

    exc[252] = '\0';

    const char *del_a = "'";
    const char *del_s = "\"";
    const char *del_b = ":";

    for (const ParaVMFunction *const *fun = paravm_get_functions(mod); *fun; fun++)
    {
        write(&sjlj, f, ".fun ");
        write_str(&sjlj, f, exc, del_s, (*fun)->name);
        write(&sjlj, f, "\n");

        for (const ParaVMRegister *const *arg = paravm_get_arguments(*fun); *arg; arg++)
        {
            write(&sjlj, f, ".arg ");
            write_str(&sjlj, f, exc, del_s, (*arg)->name);
            write(&sjlj, f, "\n");
        }

        for (const ParaVMRegister *const *reg = paravm_get_registers(*fun); *reg; reg++)
        {
            if ((*reg)->argument)
                continue;

            write(&sjlj, f, ".reg ");
            write_str(&sjlj, f, exc, del_s, (*reg)->name);
            write(&sjlj, f, "\n");
        }

        for (const ParaVMBlock *const *blk = paravm_get_blocks(*fun); *blk; blk++)
        {
            write(&sjlj, f, ".blk ");
            write_str(&sjlj, f, exc, del_s, (*blk)->name);
            write(&sjlj, f, "\n");

            if ((*blk)->handler)
            {
                write(&sjlj, f, ".unw ");
                write_str(&sjlj, f, exc, del_s, (*blk)->handler->name);
                write(&sjlj, f, "\n");
            }

            for (const ParaVMInstruction *const *ins = paravm_get_instructions(*blk); *ins; ins++)
            {
                write(&sjlj, f, (*ins)->opcode->name);

                for (const ParaVMRegister *const *reg = (*ins)->registers; *reg; reg++)
                {
                    write_str(&sjlj, f, exc, del_s, (*reg)->name);

                    if (*(reg + 1))
                        write(&sjlj, f, " ");
                }

                if ((*ins)->opcode->operand != PARAVM_OPERAND_TYPE_NONE)
                {
                    write(&sjlj, f, " (");

                    switch ((*ins)->opcode->operand)
                    {
                        case PARAVM_OPERAND_TYPE_INTEGER:
                        case PARAVM_OPERAND_TYPE_FLOAT:
                            write(&sjlj, f, (*ins)->operand.string);

                            break;
                        case PARAVM_OPERAND_TYPE_ATOM:
                            write_str(&sjlj, f, exc, del_a, (*ins)->operand.string);

                            break;
                        case PARAVM_OPERAND_TYPE_BINARY:
                            write_str(&sjlj, f, null, del_b, (*ins)->operand.string);

                            break;
                        case PARAVM_OPERAND_TYPE_BLOCK:
                            write_str(&sjlj, f, exc, del_s, (*ins)->operand.block->name);

                            break;
                        case PARAVM_OPERAND_TYPE_BLOCKS:
                            write_str(&sjlj, f, exc, del_s, (*ins)->operand.blocks[0]->name);
                            write(&sjlj, f, " ");
                            write_str(&sjlj, f, exc, del_s, (*ins)->operand.blocks[1]->name);

                            break;
                        default:
                            assert_unreachable();
                            break;
                    }

                    write(&sjlj, f, ")");
                }

                write(&sjlj, f, "\n");
            }
        }

        if (*(fun + 1))
            write(&sjlj, f, "\n");
    }

    fclose(f);
    return PARAVM_ERROR_OK;
}
