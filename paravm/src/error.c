#include "error.h"

const char *paravm_error_to_string(ParaVMError err)
{
    switch (err)
    {
        case PARAVM_ERROR_OK:
            return null;
        case PARAVM_ERROR_BAD_UTF8:
            return "Input UTF-8 is malformed";
        case PARAVM_ERROR_SYNTAX:
            return "Syntax error encountered";
        case PARAVM_ERROR_OVERFLOW:
            return "Value is too large to be representable";
        case PARAVM_ERROR_NAME_EXISTS:
            return "Name is already mapped to a value";
        case PARAVM_ERROR_READ_ONLY:
            return "File system is in read-only mode";
        case PARAVM_ERROR_ACCESS:
            return "Insufficient permissions for file system entry";
        case PARAVM_ERROR_IO_INTERRUPT:
            return "I/O operation was interrupted";
        case PARAVM_ERROR_DIRECTORY:
            return "Path pointed to a directory rather than a file";
        case PARAVM_ERROR_LIMIT:
            return "System limit reached";
        case PARAVM_ERROR_PATH_LENGTH:
            return "Path is too long";
        case PARAVM_ERROR_NONEXISTENT:
            return "Part of the path does not exist";
        case PARAVM_ERROR_NO_SPACE:
            return "File system is full";
        case PARAVM_ERROR_NOT_DIRECTORY:
            return "Part of the path is not a directory";
        case PARAVM_ERROR_BUSY:
            return "File could not be written because it is in use";
        case PARAVM_ERROR_ASSEMBLY:
            return "Assembler error encountered";
        case PARAVM_ERROR_VERSION:
            return "Module version is newer than the VM version";
        case PARAVM_ERROR_IO:
            return "Physical I/O error occurred";
        case PARAVM_ERROR_EOF:
            return "End-of-file reached unexpectedly";
        case PARAVM_ERROR_NONEXISTENT_NAME:
            return "Name is not mapped to a value";
        default:
            assert_unreachable();
            return null;
    }
}
