#include <memory/mm.h>

static void* realloc(void* ptr, size_t newsize) {
    error_t err = NO_ERROR;
    void* new_ptr = ptr;

    CHECK_AND_RETHROW(mm_reallocate(&new_ptr, newsize));

cleanup:
    if(err != NO_ERROR) {
        CATCH(mm_free(ptr));
        return NULL;
    }
    return new_ptr;
}

static void free(void* ptr) {
    error_t err = NO_ERROR;

    // we want to act like libc does 
    // with null pointers
    if(ptr == NULL) goto cleanup;

    CHECK_AND_RETHROW(mm_free(ptr));

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);        
    }
    return;
}

#define STB_DS_IMPLEMENTATION



#include "stb_ds.h"

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
#include "stb_sprintf.h"