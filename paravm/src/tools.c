#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib-2.0/glib.h>

#include "internal/options.h"
#include "internal/tools.h"

#include "lexer.h"

int asm_tool(int argc, char *argv[])
{
    assert(argv);

    if (!argc)
    {
        printf("Error: No input file given\n");
        return 1;
    }

    const char *file = argv[0];

    char *source = null;
    GError *error = null;

    if (!g_file_get_contents(file, &source, null, &error))
    {
        printf("Error: %s\n", error->message);
        return 1;
    }

    free(error);

    ParaVMToken *tokens;
    uint32_t line;
    uint32_t column;

    ParaVMError lex_err = paravm_lex_string(source, &tokens, &line, &column);

    free(source);

    if (lex_err == PARAVM_ERROR_BAD_UTF8)
    {
        printf("Error: file '%s' contains bad UTF-8\n", file);
        return 1;
    }

    if (lex_err == PARAVM_ERROR_SYNTAX)
    {
        printf("Error: Syntax error in '%s' (near line %i, column %i)\n", file, line, column);
        return 1;
    }

    free(tokens);

    return 0;
}

int dis_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

int chk_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

int opt_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

int exe_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

int dbg_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

int chg_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

    return 0;
}

typedef struct {
    const char *name;
    ToolFunc func;
} Tool;

static Tool tools[] =
{
    { "asm", &asm_tool },
    { "dis", &dis_tool },
    { "chk", &chk_tool },
    { "opt", &opt_tool },
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
