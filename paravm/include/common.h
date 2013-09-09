#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdalign.h>

#define paravm_attr(X) __attribute__((X))

#define paravm_api paravm_attr(visibility("default"))
#define paravm_deprecated(MSG) paravm_attr(deprecated(MSG))
#define paravm_nonnull(...) paravm_attr(nonnull(__VA_ARGS__))
#define paravm_nodiscard paravm_attr(warn_unused_result)
#define paravm_nothrow paravm_attr(nothrow)

#define paravm_hot paravm_attr(hot)
#define paravm_cold paravm_attr(cold)
#define paravm_const paravm_attr(const)
#define paravm_pure paravm_attr(pure)
#define paravm_inline paravm_attr(always_inline)
#define paravm_noinline paravm_attr(noinline)
#define paravm_noclone paravm_attr(noclone)

#define paravm_aligned(X) attr(aligned(X))
#define paravm_packed attr(packed)
