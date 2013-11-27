#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "internal/option.h"
#include "internal/tool.h"

#include "assemble.h"
#include "disassemble.h"
#include "io.h"
#include "verify.h"

static const char pva_ext[] = ".pva";
static const char pvc_ext[] = ".pvc";
static const char hdf_ext[] = ".hdf";
static const char pid_ext[] = ".pid";

static int check_path(const char *path, const char *ext)
{
    assert(path);
    assert(ext);

    char *base = g_path_get_basename(path);
    const char *dot = strrchr(base, '.');

    if (!strcmp(base, "/"))
    {
        g_fprintf(stderr, "Error: File '%s' does not have a base name\n", path);
        g_free(base);
        return 1;
    }

    if (!dot || strcmp(dot, ext))
    {
        g_fprintf(stderr, "Error: File '%s' does not end in '%s'\n", path, ext);
        g_free(base);
        return 1;
    }

    if (dot == base)
    {
        g_fprintf(stderr, "Error: File '%s' does not have a root name\n", path);
        g_free(base);
        return 1;
    }

    return 0;
}

static const ParaVMModule *read_module(const char *path)
{
    char *name = paravm_extract_module_name(path);
    const ParaVMModule *mod = paravm_create_module(name);
    g_free(name);

    ParaVMError io_err = paravm_read_module(path, mod);

    if (io_err != PARAVM_ERROR_OK)
        paravm_destroy_module(mod);

    if (io_err == PARAVM_ERROR_FOURCC)
    {
        g_fprintf(stderr, "Error: Could not read '%s': File is not a PVC module\n", path);
        return null;
    }

    if (io_err == PARAVM_ERROR_NAME_EXISTS ||
        io_err == PARAVM_ERROR_NONEXISTENT_NAME)
    {
        g_fprintf(stderr, "Error: Could not read '%s': Module contains invalid code\n", path);
        return null;
    }

    if (io_err != PARAVM_ERROR_OK)
    {
        g_fprintf(stderr, "Error: Could not read '%s': %s\n", path, paravm_error_to_string(io_err));
        return null;
    }

    return mod;
}

static int write_module(const char *path, const ParaVMModule *mod)
{
    char *out_name;

    if (!opt_out)
    {
        out_name = g_strdup(path);
        strncpy(out_name + strlen(out_name) - sizeof(pvc_ext) + 1, pvc_ext, sizeof(pvc_ext) - 1);
    }
    else
        out_name = g_strdup(opt_out);

    ParaVMError io_err = paravm_write_module(mod, out_name);

    if (io_err != PARAVM_ERROR_OK)
    {
        g_fprintf(stderr, "Error: Could not write '%s': %s\n", out_name, paravm_error_to_string(io_err));
        g_free(out_name);
        return 1;
    }

    g_free(out_name);
    return 0;
}

int asm_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        g_fprintf(stderr, "Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    if (check_path(file, pva_ext))
        return 1;

    if (opt_out && check_path(opt_out, pvc_ext))
        return 1;

    gchar *source = null;
    GError *error = null;

    if (!g_file_get_contents(file, &source, null, &error))
    {
        g_fprintf(stderr, "Error: %s\n", error->message);
        return 1;
    }

    g_free(error);

    ParaVMToken **tokens;
    uint32_t line;
    uint32_t column;

    ParaVMError lex_err = paravm_lex_string(source, &tokens, &line, &column);

    g_free(source);

    if (lex_err == PARAVM_ERROR_BAD_UTF8)
    {
        g_fprintf(stderr, "Error: File '%s' contains bad UTF-8\n", file);
        return 1;
    }

    if (lex_err == PARAVM_ERROR_SYNTAX)
    {
        g_fprintf(stderr, "Error: Syntax error in '%s' (near line %i, column %i)\n", file, line, column);
        return 1;
    }

    if (lex_err == PARAVM_ERROR_OVERFLOW)
    {
        g_fprintf(stderr, "Error: Floating point overflow in '%s' (near line %i, column %i)\n", file, line, column);
        return 1;
    }

    char *name = paravm_extract_module_name(file);
    const ParaVMModule *mod = paravm_create_module(name);
    g_free(name);

    ParaVMError asm_err = paravm_assemble_tokens(tokens, mod, &line, &column);

    for (ParaVMToken **tok = &tokens[0]; *tok; tok++)
        paravm_destroy_token(*tok);

    g_free(tokens);

    if (asm_err != PARAVM_ERROR_OK)
        paravm_destroy_module(mod);

    if (asm_err == PARAVM_ERROR_SYNTAX)
    {
        g_fprintf(stderr, "Error: Syntax error in '%s' (near line %i, column %i)\n", file, line, column);
        return 1;
    }

    if (asm_err == PARAVM_ERROR_ASSEMBLY)
    {
        g_fprintf(stderr, "Error: Assembly error in '%s' (near line %i, column %i)\n", file, line, column);
        return 1;
    }

    int res = write_module(file, mod);

    paravm_destroy_module(mod);

    return res;
}

int dis_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        g_fprintf(stderr, "Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    if (check_path(file, pvc_ext))
        return 1;

    if (opt_out && check_path(opt_out, pva_ext))
        return 1;

    const ParaVMModule *mod;

    if (!(mod = read_module(file)))
        return 1;

    char *out_name;

    if (!opt_out)
    {
        out_name = g_strdup(file);
        strncpy(out_name + strlen(out_name) - sizeof(pva_ext) + 1, pva_ext, sizeof(pva_ext) - 1);
    }
    else
        out_name = g_strdup(opt_out);

    ParaVMError io_err = paravm_disassemble_module(mod, out_name);

    if (io_err != PARAVM_ERROR_OK)
    {
        g_fprintf(stderr, "Error: Could not write '%s': %s\n", out_name, paravm_error_to_string(io_err));
        g_free(out_name);
        return 1;
    }

    g_free(out_name);

    return 0;
}

int chk_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        g_fprintf(stderr, "Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    if (check_path(file, pvc_ext))
        return 1;

    const ParaVMModule *mod;

    if (!(mod = read_module(file)))
        return 1;

    const ParaVMFunction *o_fun;
    const ParaVMBlock *o_blk;
    const ParaVMInstruction *o_insn;

    ParaVMVerifierResult ver_res = paravm_verify_module(mod, &o_fun, &o_blk, &o_insn);

    int res = ver_res != PARAVM_VERIFIER_OK;

    if (ver_res == PARAVM_VERIFIER_NO_TERMINATOR)
        g_fprintf(stderr, "Error: Block '%s' in function '%s' has no terminator\n", o_blk->name, o_fun->name);

    if (ver_res == PARAVM_VERIFIER_MULTIPLE_TERMINATORS)
        g_fprintf(stderr, "Error: Block '%s' in function '%s' has multiple terminators\n", o_blk->name, o_fun->name);

    if (ver_res == PARAVM_VERIFIER_BAD_ENDIANNESS)
    {
        g_fprintf(stderr, "Error: Instruction %s at %zu in %s has bad endianness operand (%s)\n",
                  o_insn->opcode->name, paravm_get_instruction_index(o_blk, o_insn),
                  o_blk->name, o_insn->operand.string);
    }

    paravm_destroy_module(mod);

    return res;
}

int exe_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        g_fprintf(stderr, "Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    if (check_path(file, pvc_ext))
        return 1;

    if (opt_hdf && check_path(opt_hdf, hdf_ext))
        return 1;

    if (opt_pid && check_path(opt_pid, pid_ext))
        return 1;

    return 0;
}

int dbg_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        g_fprintf(stderr, "Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    if (check_path(file, pid_ext))
        return 1;

    return 0;
}

int chg_tool(int argc, char *argv[])
{
    assert(argv);

    if (argc < 2)
    {
        g_fprintf(stderr, "Error: No input files given\n");
        return 1;
    }

    const char *pid_file = argv[0];
    const char *pvc_file = argv[1];

    if (check_path(pid_file, pid_ext))
        return 1;

    if (check_path(pvc_file, pvc_ext))
        return 1;

    return 0;
}

typedef struct
{
    const char *name;
    ToolFunc func;
} Tool;

static Tool tools[] =
{
    { "asm", &asm_tool },
    { "dis", &dis_tool },
    { "chk", &chk_tool },
    { "exe", &exe_tool },
    { "dbg", &dbg_tool },
    { "chg", &chg_tool },
    { null, null },
};

ToolFunc find_tool(const char *name)
{
    assert(name);

    for (Tool *t = &tools[0]; t->name; t++)
        if (!strcmp(t->name, name))
            return t->func;

    return null;
}
