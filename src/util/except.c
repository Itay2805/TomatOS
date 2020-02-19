#include "except.h"
#include "def.h"

static const char* errors[] = {
    [NO_ERROR] = "No error",
    [ERROR_CHECK_FAILED] = "Check failed",
    [ERROR_NOT_READY] = "Not ready",
    [ERROR_INVALID_PARAM] = "Invalid parameter",
    [ERROR_OUT_OF_RESOURCES] = "Out of resources",
    [ERROR_NOT_FOUND] = "Not found",
};

const char* strerror(err_t err) {
    if (ARRAY_LEN(errors) <= err) {
        return "Unknown error";
    }
    return errors[err];
}

void panic(const char* panic_message) {
    trace(panic_message);
    while(1);
}
