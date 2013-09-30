#pragma once

typedef typeof(int (int argc, char *argv[])) *ToolFunc;

paravm_nothrow
paravm_nonnull()
ToolFunc find_tool(const char *name);
