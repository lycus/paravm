#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdalign.h>

/* Alias for `__attribute__((X))`.
 */
#define paravm_attr(X) __attribute__((X))

/* Marks a function as public ParaVM API.
 *
 * Equivalent to `__attribute__((visibility("default")))`.
 */
#define paravm_api paravm_attr(visibility("default"))

/* Marks something as deprecated with a message.
 *
 * Equivalent to `__attribute__((deprecated(MSG)))`.
 */
#define paravm_deprecated(MSG) paravm_attr(deprecated(MSG))

/* Specifies that the given parameters must not be
 * `NULL`. If no arguments are given to this macro, it
 * affects all pointer parameters.
 *
 * Equivalent to `__attribute__((nonnull(...)))`.
 */
#define paravm_nonnull(...) paravm_attr(nonnull(__VA_ARGS__))

/* Specifies that a function does not throw exceptions.
 * This is the case for all ParaVM API functions.
 *
 * Equivalent to `__attribute__((nothrow))`.
 */
#define paravm_nothrow paravm_attr(nothrow)

/* Specifies that a function should always be inlined.
 *
 * Equivalent `__attribute__((always_inline))`.
 */
#define paravm_inline paravm_attr(always_inline)

/* Marks a structure as tightly packed.
 *
 * Equivalent to `__attribute__((packed))`.
 */
#define paravm_packed attr(packed)

/* Expands to `extern "C" { ... }`.
 *
 * Should be used in all headers containing function
 * and variable declarations so that they are correctly
 * treated as C functions in C++ code.
 */
#if defined(__cplusplus)
#define paravm_begin extern "C" {
#define paravm_end }
#else
#define paravm_begin
#define paravm_end
#endif
