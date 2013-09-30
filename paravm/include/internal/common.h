#pragma once

#include <stddef.h>

#include "../common.h"

#define paravm_internal paravm_attr(visibility("default"))
#define thread_local _Thread_local
#define var_used paravm_attr(used)
#define var_unused paravm_attr(unused)
#define naked_func paravm_attr(naked)
#define global_ctor paravm_attr(constructor)
#define global_dtor paravm_attr(destructor)

#define null ((void *)0)

#define expect(CND) __builtin_expect(CND, true)
#define prefetch(PTR, WRT) __builtin_prefetch(PTR, WRT)

#define bitsof(X) (sizeof(X) * 8)

#define _STRINGIFY(X) #X
#define STRINGIFY(X) _STRINGIFY(X)

paravm_internal
paravm_nothrow
paravm_nonnull()
noreturn
void paravm_assert(const char *file, int line, const char *func, const char *msg);

#if defined(PARAVM_DEBUG)
#    define assert(CND) \
        if (!expect(!!(CND))) \
            paravm_assert(__FILE__, __LINE__, __func__, STRINGIFY(CND))
#    define assert_unreachable() paravm_assert(__FILE__, __LINE__, __func__, "should not be reached")
#else
#    define assert(CND)
#    define assert_unreachable()
#endif

#define static_assert(CND) _Static_assert(CND, STRINGIFY(CND))
