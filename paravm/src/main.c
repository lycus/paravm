#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "internal/tools.h"

int version;
int help;
int lint;
int emu;
const char *out;
const char *pid;

static const struct option options[] =
{
    { "help", no_argument, &help, true },
    { "version", no_argument, &version, true },
    { "lint", no_argument, &lint, true },
    { "emu", no_argument, &emu, true },
    { "out", required_argument, null, 'o' },
    { "pid", required_argument, null, 'p' },
    { null, 0, null, 0 },
};

void usage(const char *program)
{
    assert(program);

    printf("Usage: %s [--version] [--help] [options] <tool> <args>\n", basename(program));
}

int main(int argc, char *argv[])
{
    assert(argc);
    assert(argv);

    while (true)
    {
        int c = getopt_long(argc, argv, "", options, null);

        if (c == -1)
            break;

        switch (c)
        {
            case 'o':
                out = optarg;
                break;
            case 'p':
                pid = optarg;
                break;
            case ':':
            case '?':
                usage(argv[0]);
                return 1;
        }
    }

    // Skip past all the options so we only have non-option arguments.
    argc -= optind;
    argv += optind;

    if (version)
    {
        printf("ParaVM - " PACKAGE_VERSION "\n");
        printf("Copyright (C) 2013 The Lycus Foundation\n");
        printf("Available under the terms of the MIT License\n");
    }

    if (help)
    {
        usage(argv[0]);
        printf("Please refer to `man 1 paravm`\n");
    }

    if (version || help)
        return 0;

    if (!argc)
    {
        printf("Error: No tool name given\n");
        return 1;
    }

    const char *tool = argv[0];
    ToolFunc func = find_tool(tool);

    if (!func)
    {
        printf("Error: Invalid tool name '%s'\n", tool);
        return 1;
    }

    return func(argc - 1, argv + 1);
}
