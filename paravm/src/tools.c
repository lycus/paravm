#include <string.h>

#include "internal/options.h"
#include "internal/tools.h"

int asm_tool(int argc var_unused, char *argv[] var_unused)
{
    assert(argv);

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
