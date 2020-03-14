#include "except.h"
#include "def.h"

static const char* errors[] = {
    [NO_ERROR] = "No error",
    [ERROR_CHECK_FAILED] = "Check failed",
    [ERROR_OUT_OF_RESOURCES] = "Out of resources",
    [ERROR_INVALID_PARAM] = "Invalid parameter",
    [ERROR_INVALID_POINTER] = "Invalid pointer",
    [ERROR_INVALID_HANDLE] = "Invalid handle",
    [ERROR_INVALID_TPL] = "Invalid Task Priority Level",
    [ERROR_NOT_READY] = "Not ready",
    [ERROR_NOT_FOUND] = "Not found",
    [ERROR_EOF] = "End of file",
};

const char* strerror(err_t err) {
    if (ARRAY_LEN(errors) <= err) {
        return "Unknown error";
    }
    return errors[err];
}
