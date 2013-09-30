#include <endian.h>
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "io.h"

const uint32_t version = 3;

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

static void write(jmp_buf *sjlj, FILE *f, const void *data, size_t size)
{
    assert(sjlj);
    assert(f);
    assert(data);

    if (!fwrite(data, size, 1, f))
        longjmp(*sjlj, 1);
}

static void write_u8(jmp_buf *sjlj, FILE *f, uint8_t value)
{
    assert(sjlj);
    assert(f);

    write(sjlj, f, &value, sizeof(uint8_t));
}

static void write_u32(jmp_buf *sjlj, FILE *f, uint32_t value)
{
    assert(sjlj);
    assert(f);

    uint32_t nvalue = htole32(value);

    write(sjlj, f, &nvalue, sizeof(uint32_t));
}

static void write_str(jmp_buf *sjlj, FILE *f, const char *value)
{
    assert(sjlj);
    assert(f);
    assert(value);

    uint32_t len = (uint32_t)strlen(value);

    write_u32(sjlj, f, len);
    write(sjlj, f, value, len);
}

static void read(jmp_buf *sjlj, FILE *f, void *dest, size_t size)
{
    assert(sjlj);
    assert(f);

    if (!fread(dest, size, 1, f))
        longjmp(*sjlj, 1);
}

static uint8_t read_u8(jmp_buf *sjlj, FILE *f)
{
    assert(sjlj);
    assert(f);

    uint8_t value;

    read(sjlj, f, &value, sizeof(uint8_t));

    return value;
}

static uint32_t read_u32(jmp_buf *sjlj, FILE *f)
{
    assert(sjlj);
    assert(f);

    uint32_t value;

    read(sjlj, f, &value, sizeof(uint32_t));

    return le32toh(value);
}

static const char *read_str(jmp_buf *sjlj, FILE *f)
{
    assert(sjlj);
    assert(f);

    uint32_t len = read_u32(sjlj, f);
    char *str = g_new(char, len + 1);

    read(sjlj, f, str, len);

    str[len] = '\0';

    return str;
}

ParaVMError paravm_write_module(const ParaVMModule *mod, const char *path)
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

    write_u32(&sjlj, f, version);
    write_u32(&sjlj, f, (uint32_t)paravm_get_function_count(mod));

    for (const ParaVMFunction *const *fun = paravm_get_functions(mod); *fun; fun++)
    {
        write_str(&sjlj, f, (*fun)->name);
        write_u32(&sjlj, f, (uint32_t)paravm_get_register_count(*fun));

        for (const ParaVMRegister *const *reg = paravm_get_registers(*fun); *reg; reg++)
        {
            write_str(&sjlj, f, (*reg)->name);
            write_u8(&sjlj, f, (*reg)->argument);
        }

        write_u32(&sjlj, f, (uint32_t)paravm_get_block_count(*fun));

        for (const ParaVMBlock *const *blk = paravm_get_blocks(*fun); *blk; blk++)
            write_str(&sjlj, f, (*blk)->name);

        for (const ParaVMBlock *const *blk = paravm_get_blocks(*fun); *blk; blk++)
        {
            write_str(&sjlj, f, (*blk)->name);
            write_u8(&sjlj, f, !!(*blk)->handler);

            if ((*blk)->handler)
                write_str(&sjlj, f, (*blk)->handler->name);

            write_u8(&sjlj, f, !!(*blk)->exception);

            if ((*blk)->exception)
                write_str(&sjlj, f, (*blk)->exception->name);

            write_u32(&sjlj, f, (uint32_t)paravm_get_instruction_count(*blk));

            for (const ParaVMInstruction *const *ins = paravm_get_instructions(*blk); *ins; ins++)
            {
                write_u8(&sjlj, f, (*ins)->opcode->code);

                if ((*ins)->register1)
                    write_str(&sjlj, f, (*ins)->register1->name);

                if ((*ins)->register2)
                    write_str(&sjlj, f, (*ins)->register2->name);

                if ((*ins)->register3)
                    write_str(&sjlj, f, (*ins)->register3->name);

                if ((*ins)->opcode->operand == PARAVM_OPERAND_TYPE_ARGS)
                {
                    uint32_t arg_c = 0;

                    for (const char *const *list = (*ins)->operand.args; *list; list++)
                        arg_c++;

                    write_u32(&sjlj, f, arg_c);

                    for (const char *const *str = (*ins)->operand.args; *str; str++)
                        write_str(&sjlj, f, *str);
                }
                else if ((*ins)->opcode->operand == PARAVM_OPERAND_TYPE_BLOCKS)
                {
                    write_str(&sjlj, f, (*ins)->operand.blocks[0]->name);
                    write_str(&sjlj, f, (*ins)->operand.blocks[1]->name);
                }
                else if ((*ins)->opcode->operand == PARAVM_OPERAND_TYPE_BLOCK)
                    write_str(&sjlj, f, (*ins)->operand.block->name);
                else if ((*ins)->opcode->operand != PARAVM_OPERAND_TYPE_NONE)
                    write_str(&sjlj, f, (*ins)->operand.string);
            }
        }
    }

    fclose(f);
    return PARAVM_ERROR_OK;
}

ParaVMError paravm_read_module(const char *path, const ParaVMModule *mod)
{
    assert(path);
    assert(mod);

    FILE *f = fopen(path, "r");

    if (!f)
        return errno_to_error(errno);

    jmp_buf sjlj;

    if (setjmp(sjlj))
    {
        ParaVMError err = feof(f) ? PARAVM_ERROR_EOF : errno_to_error(errno);

        fclose(f);

        return err;
    }

    uint32_t mod_ver = read_u32(&sjlj, f);

    if (mod_ver > version)
    {
        fclose(f);
        return PARAVM_ERROR_VERSION;
    }

    ParaVMError err = PARAVM_ERROR_OK;

    uint32_t fun_c = read_u32(&sjlj, f);

    for (uint32_t i = 0; i < fun_c; i++)
    {
        const char *str = read_str(&sjlj, f);

        const ParaVMFunction *fun = paravm_create_function(str);
        g_free((char *)str);

        if ((err = paravm_add_function(mod, fun)) != PARAVM_ERROR_OK)
        {
            paravm_destroy_function(fun);
            fclose(f);
            return err;
        }

        uint32_t reg_c = read_u32(&sjlj, f);

        for (uint32_t j = 0; j < reg_c; j++)
        {
            const char *reg_str = read_str(&sjlj, f);
            bool arg = read_u8(&sjlj, f);

            const ParaVMRegister *reg = paravm_create_register(reg_str, arg);
            g_free((char *)reg_str);

            if ((err = paravm_add_register(fun, reg)) != PARAVM_ERROR_OK)
            {
                paravm_destroy_register(reg);
                fclose(f);
                return err;
            }
        }

        uint32_t blk_c = read_u32(&sjlj, f);

        for (uint32_t j = 0; j < blk_c; j++)
        {
            const char *blk_str = read_str(&sjlj, f);

            const ParaVMBlock *blk = paravm_create_block(blk_str);
            g_free((char *)blk_str);

            if ((err = paravm_add_block(fun, blk)) != PARAVM_ERROR_OK)
            {
                paravm_destroy_block(blk);
                fclose(f);
                return err;
            }
        }

        for (uint32_t j = 0; j < blk_c; j++)
        {
            const char *blk_str = read_str(&sjlj, f);

            const ParaVMBlock *blk = paravm_get_block(fun, blk_str);
            g_free((char *)blk_str);

            if (!blk)
            {
                fclose(f);
                return PARAVM_ERROR_NONEXISTENT_NAME;
            }

            bool has_unw = read_u8(&sjlj, f);

            if (has_unw)
            {
                const char *unw_str = read_str(&sjlj, f);

                const ParaVMBlock *unw_blk = paravm_get_block(fun, unw_str);
                g_free((char *)unw_str);

                if (!unw_blk)
                {
                    fclose(f);
                    return PARAVM_ERROR_NONEXISTENT_NAME;
                }

                paravm_set_handler_block(blk, unw_blk);
            }

            bool has_exc = read_u8(&sjlj, f);

            if (has_exc)
            {
                const char *exc_str = read_str(&sjlj, f);

                const ParaVMRegister *exc_reg = paravm_get_register(fun, exc_str);
                g_free((char *)exc_str);

                if (!exc_reg)
                {
                    fclose(f);
                    return PARAVM_ERROR_NONEXISTENT_NAME;
                }

                paravm_set_exception_register(blk, exc_reg);
            }

            uint32_t ins_c = read_u32(&sjlj, f);

            for (uint32_t k = 0; k < ins_c; k++)
            {
                uint8_t code = read_u8(&sjlj, f);

                const ParaVMOpCode *opc = paravm_get_opcode_by_code(code);

                if (!opc)
                {
                    fclose(f);
                    return PARAVM_ERROR_NONEXISTENT_NAME;
                }

#define READ_REG(NUM) \
    const ParaVMRegister *reg ## NUM = null; \
    \
    if (opc->registers >= NUM) \
    { \
        const char *reg_str = read_str(&sjlj, f); \
        \
        reg ## NUM = paravm_get_register(fun, reg_str); \
        g_free((char *)reg_str); \
        \
        if (!reg ## NUM) \
        { \
            fclose(f); \
            return PARAVM_ERROR_NONEXISTENT_NAME; \
        } \
    }

                READ_REG(1);
                READ_REG(2);
                READ_REG(3);

                ParaVMOperand operand;
                bool own_operand = false;

                if (opc->operand == PARAVM_OPERAND_TYPE_ARGS)
                {
                    uint32_t oper_c = read_u32(&sjlj, f);

                    const char **arr = g_new(const char *, oper_c + 1);

                    for (uint32_t l = 0; l < oper_c; l++)
                        arr[l] = read_str(&sjlj, f);

                    arr[oper_c] = null;

                    operand.args = arr;
                    own_operand = true;
                }
                else if (opc->operand == PARAVM_OPERAND_TYPE_BLOCKS)
                {
                    const char *blk_arg_str1 = read_str(&sjlj, f);
                    const char *blk_arg_str2 = read_str(&sjlj, f);

                    const ParaVMBlock *op_blk1 = paravm_get_block(fun, blk_arg_str1);
                    const ParaVMBlock *op_blk2 = paravm_get_block(fun, blk_arg_str2);

                    g_free((char *)blk_arg_str1);
                    g_free((char *)blk_arg_str2);

                    if (!op_blk1 || !op_blk2)
                    {
                        fclose(f);
                        return PARAVM_ERROR_NONEXISTENT_NAME;
                    }

                    operand.blocks[0] = op_blk1;
                    operand.blocks[1] = op_blk2;
                }
                else if (opc->operand == PARAVM_OPERAND_TYPE_BLOCK)
                {
                    const char *blk_arg_str = read_str(&sjlj, f);

                    const ParaVMBlock *op_blk = paravm_get_block(fun, blk_arg_str);

                    g_free((char *)blk_arg_str);

                    if (!op_blk)
                    {
                        fclose(f);
                        return PARAVM_ERROR_NONEXISTENT_NAME;
                    }

                    operand.block = op_blk;
                }
                else if (opc->operand != PARAVM_OPERAND_TYPE_NONE)
                {
                    operand.string = read_str(&sjlj, f);
                    own_operand = true;
                }

                const ParaVMInstruction *ins = paravm_create_instruction(opc,
                                                                         operand,
                                                                         own_operand,
                                                                         reg1,
                                                                         reg2,
                                                                         reg3);

                if (opc->operand == PARAVM_OPERAND_TYPE_ARGS)
                {
                    for (const char *const *ptr = operand.args; *ptr; ptr++)
                        g_free(*(void **)ptr);

                    g_free((void *)operand.raw);
                }
                else if (opc->operand != PARAVM_OPERAND_TYPE_BLOCK &&
                         opc->operand != PARAVM_OPERAND_TYPE_BLOCKS)
                    g_free((void *)operand.raw);

                paravm_append_instruction(blk, ins);
            }
        }
    }

    fclose(f);
    return PARAVM_ERROR_OK;
}

static const char pva_ext[] = ".pva";
static const char pvc_ext[] = ".pvc";

char *paravm_extract_module_name(const char *path)
{
    const char *base = basename(path);
    const char *dot = strrchr(base, '.');

    if (!dot || (strcmp(dot, pva_ext) && strcmp(dot, pvc_ext)))
        return null;

    if (dot == base)
        return null;

    return g_strndup(base, strlen(base) - sizeof(pva_ext) + 1);
}
