#pragma once

typedef typeof(int (int argc, char *argv[])) *ToolFunc;

ToolFunc find_tool(const char *name) paravm_nonnull(1);
