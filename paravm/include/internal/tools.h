#pragma once

typedef int (* ToolFunc)(int argc, char *argv[]);

ToolFunc find_tool(const char *name) paravm_nonnull(1);
