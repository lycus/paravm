#include <getopt.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "internal/tool.h"

int opt_version;
int opt_help;
int opt_emu;
const char *opt_entry;
const char *opt_hdf;
const char *opt_out;
const char *opt_pid;

static const struct option options[] =
{
    { "help", no_argument, &opt_help, true },
    { "version", no_argument, &opt_version, true },
    { "emu", no_argument, &opt_emu, true },
    { "entry", required_argument, null, 'e' },
    { "hdf", required_argument, null, 'h' },
    { "out", required_argument, null, 'o' },
    { "pid", required_argument, null, 'p' },
    { null, 0, null, 0 },
};

void usage(const char *program)
{
    assert(program);

    const char *base = g_path_get_basename(program);

    g_fprintf(stderr, "Usage: %s [--version] [--help] [options] <tool> <args>\n", base);

    g_free((char *)base);
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
            case 'e':
                opt_entry = optarg;
                break;
            case 'h':
                opt_hdf = optarg;
                break;
            case 'o':
                opt_out = optarg;
                break;
            case 'p':
                opt_pid = optarg;
                break;
            case ':':
            case '?':
                usage(argv[0]);
                return 1;
        }
    }

    if (opt_version)
    {
        g_fprintf(stderr, "ParaVM - " PACKAGE_VERSION "\n");
        g_fprintf(stderr, "Copyright (C) 2013 The Lycus Foundation\n");
        g_fprintf(stderr, "Available under the terms of the MIT License\n");
    }

    if (opt_help)
    {
        usage(argv[0]);
        g_fprintf(stderr, "Please refer to `man 1 paravm`\n");
    }

    if (opt_version || opt_help)
        return 0;

    // Skip past all the options so we only have non-option arguments.
    argc -= optind;
    argv += optind;

    if (!argc)
    {
        g_fprintf(stderr, "Error: No tool name given\n");
        return 1;
    }

    const char *tool = argv[0];
    ToolFunc func = find_tool(tool);

    if (!func)
    {
        g_fprintf(stderr, "Error: Invalid tool name '%s'\n", tool);
        return 1;
    }

    return func(argc - 1, argv + 1);
}
