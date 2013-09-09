#include <stdio.h>
#include <stdlib.h>

void paravm_assert(bool cond, const char *file, int line,
                   const char *func, const char *msg)
{
    if (!expect(cond)) \
    {
        printf("Assertion failed at %s:%i!%s: %s\n", file, line, func, msg);
        abort();
    }
}
