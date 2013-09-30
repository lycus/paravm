#pragma once

#include "common.h"

paravm_begin

typedef enum ParaVMError ParaVMError;

/* Defines the various errors that ParaVM APIs can return.
 */
enum ParaVMError
{
    PARAVM_ERROR_OK = 0, // No error.
    PARAVM_ERROR_BAD_UTF8 = 1, // Input UTF-8 was malformed.
    PARAVM_ERROR_SYNTAX = 2, // A syntax error was encountered.
    PARAVM_ERROR_OVERFLOW = 3, // A value was too large to be representable.
    PARAVM_ERROR_NAME_EXISTS = 4, // A name was already mapped to a value.
    PARAVM_ERROR_READ_ONLY = 5, // A file system was in read-only mode.
    PARAVM_ERROR_ACCESS = 6, // Insufficient permissions for a file system entry.
    PARAVM_ERROR_IO_INTERRUPT = 7, // An I/O operation was interrupted.
    PARAVM_ERROR_DIRECTORY = 8, // A path pointed to a directory rather than a file.
    PARAVM_ERROR_LIMIT = 9, // A system limit was reached.
    PARAVM_ERROR_PATH_LENGTH = 10, // A path was too long.
    PARAVM_ERROR_NONEXISTENT = 11, // A file system entry did not exist.
    PARAVM_ERROR_NO_SPACE = 12, // A file system was full.
    PARAVM_ERROR_NOT_DIRECTORY = 13, // A part of a path was not a directory.
    PARAVM_ERROR_BUSY = 14, // A file could not be written because it was in use.
    PARAVM_ERROR_ASSEMBLY = 15, // An assembler error was encountered.
    PARAVM_ERROR_VERSION = 16, // Module version was newer than the VM version.
    PARAVM_ERROR_IO = 17, // A physical I/O error occurred.
    PARAVM_ERROR_EOF = 18, // End-of-file was reached unexpectedly.
    PARAVM_ERROR_NONEXISTENT_NAME = 19, // A name was not mapped to a value.
    PARAVM_ERROR_ALREADY_SET = 20, // A property was already set.
};

/* Gets a static string describing `err`. Returns a `NULL`
 * pointer for `PARAVM_ERROR_OK`.
 */
paravm_api
paravm_nothrow
const char *paravm_error_to_string(ParaVMError err);

paravm_end
