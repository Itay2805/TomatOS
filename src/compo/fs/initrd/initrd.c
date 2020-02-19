#include <stddef.h>
#include <stdbool.h>
#include <util/except.h>
#include <stb_ds.h>
#include <util/def.h>
#include <sync/spinlock.h>
#include <compo/component.h>
#include <compo/fs/filesystem.h>
#include <mm/mm.h>
#include <mm/vmm.h>

#include "initrd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tar related stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TAR_HEADER_SIZE 512

typedef struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
} tar_header_t;

static size_t tar_size(tar_header_t* header) {
    size_t size = 0;
    size_t count = 1;

    for(int j = 11; j > 0; j--, count *= 8){
        size += ((header->size[j - 1] - '0') * count);
    }

    return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct initrd_file {
    const char* key;
    tar_header_t* value;
}* initrd_files = NULL;

static spinlock_t lock = SPINLOCK_INIT;

typedef struct fd_entry {
    char* data;
    size_t seek;
    size_t size;
    spinlock_t lock;
} fd_entry_t;

static err_t tar_open(component_t* ctx, const char* path, file_t* handle) {
    err_t err = NO_ERROR;

    spinlock_acquire(&lock);

    CHECK_ERROR(path != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);

    int i = shgeti(initrd_files, path);
    CHECK_ERROR(i != -1, ERROR_NOT_FOUND);
    tar_header_t* header = initrd_files[i].value;

    fd_entry_t* entry = mm_allocate(sizeof(fd_entry_t));
    entry->data = (void*)((uintptr_t)header + TAR_HEADER_SIZE);
    entry->seek = 0;
    entry->size = tar_size(header);

    *handle = entry;

cleanup:
    spinlock_release(&lock);
    return err;
}

static err_t tar_close(component_t* ctx, file_t file) {
    err_t err = NO_ERROR;

    CHECK(file != NULL);
    mm_free(file);

cleanup:
    return err;
}

static err_t tar_eof(struct component* comp, file_t handle, bool* iseof) {
    err_t err = NO_ERROR;
    fd_entry_t* file = handle;

    CHECK(handle != NULL);

    spinlock_acquire(&file->lock);

    CHECK_ERROR(iseof != NULL, ERROR_INVALID_PARAM);
    *iseof = file->seek == file->size;

cleanup:
    if (file != NULL) {
        spinlock_release(&file->lock);
    }

    return err;
}

// TODO: overflow check on offset
static err_t tar_seek(struct component* comp, file_t handle, size_t offset, seek_type_t type) {
    err_t err = NO_ERROR;
    fd_entry_t* file = handle;

    CHECK(handle != NULL);

    spinlock_acquire(&file->lock);

    switch (type) {
        case SEEK_CUR:
            CHECK_ERROR(file->seek + offset <= file->size, ERROR_EOF);
            file->seek += offset;
            break;

        case SEEK_SET:
            CHECK_ERROR(file->size >= offset, ERROR_EOF);
            file->seek = offset;
            break;

        case SEEK_END:
            CHECK_ERROR(file->size >= offset, ERROR_EOF);
            file->seek = file->size - offset;
            break;

        default:
            CHECK_FAIL();
    }

cleanup:
    if (file != NULL) {
        spinlock_release(&file->lock);
    }

    return err;
}

err_t tar_tell(struct component* comp, file_t handle, size_t* size) {
    err_t err = NO_ERROR;
    fd_entry_t* file = handle;

    CHECK(handle != NULL);

    spinlock_acquire(&file->lock);

    CHECK_ERROR(size != NULL, ERROR_NOT_FOUND);
    *size = file->seek;

cleanup:
    if (file != NULL) {
        spinlock_release(&file->lock);
    }

    return err;
}

err_t tar_read(struct component* comp, file_t handle, void* buffer, size_t* count) {
    err_t err = NO_ERROR;
    fd_entry_t* file = handle;

    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(count != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(buffer != NULL, ERROR_INVALID_PARAM);

    spinlock_acquire(&file->lock);

    // check if eof
    CHECK_ERROR(file->seek < file->size, ERROR_EOF);

    // truncate if needed
    if (*count > file->size - file->seek) {
        *count = file->size - file->seek;
    }

    // copy it
    memcpy(buffer, &file->data[file->seek], *count);

cleanup:
    if (file != NULL) {
        spinlock_release(&file->lock);
    }

    return err;
}

static err_t tar_is_readonly(component_t* ctx, bool* is_ro) {
    err_t err = NO_ERROR;

    CHECK_ERROR(is_ro, ERROR_INVALID_PARAM);
    *is_ro = true;

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Component implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static component_t component = {
    .type = COMPONENT_FILESYSTEM,
    .fs = {
        .open = tar_open,
        .is_readonly = tar_is_readonly,

        .eof = tar_eof,
        .seek = tar_seek,
        .tell = tar_tell,
        .read = tar_read,
        .close = tar_close,
    }
};

void create_initrd_fs(tboot_module_t* module) {
    uintptr_t base = PHYSICAL_TO_DIRECT(module->base);
    tar_header_t* header = PHYSICAL_TO_DIRECT((tar_header_t*)module->base);

    // parse the initrd
    while (true) {
        // got to end of file
        if((ptrdiff_t)(((uintptr_t)header + TAR_HEADER_SIZE) - base) > (ptrdiff_t)module->len) {
            break;
        }

        // invalid name
        if (header->filename[0] == '\0') {
            break;
        }

        // make sure the file size is all good
        size_t size = ALIGN_UP(tar_size(header), TAR_HEADER_SIZE);
        ASSERT((ptrdiff_t)(((uintptr_t)header + TAR_HEADER_SIZE + size) - base) <= (ptrdiff_t)module->len);

        // add it
        shput(initrd_files, header->filename, header);

        // next
        header = (tar_header_t*)((uintptr_t)header + TAR_HEADER_SIZE + size);
    }

    // generate the address
    generate_device_address("initrd", sizeof("initrd") - 1, &component.address);

    // register component and set as primary for now
    register_component(&component);
    set_primary(&component);
}
