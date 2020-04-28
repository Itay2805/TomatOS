#include <stddef.h>
#include <stdbool.h>
#include <util/except.h>
#include <stb_ds.h>
#include <util/def.h>
#include <sync/spinlock.h>
#include <compo/component.h>
#include <compo/fs/fs.h>
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
// File implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct initrd_file {
    const char* key;
    tar_header_t* value;
}* initrd_files = NULL;

static spinlock_t lock = SPINLOCK_INIT;

typedef struct fd_entry {
    // the file interface
    struct file file;

    // the data related to it
    char* data;
    size_t seek;
    size_t size;
    spinlock_t lock;
} fd_entry_t;

static err_t tar_close(file_t file) {
    err_t err = NO_ERROR;

    mm_free(file);

cleanup:
    return err;
}

static err_t tar_eof(file_t handle, bool* iseof) {
    err_t err = NO_ERROR;
    fd_entry_t* file = (fd_entry_t*)handle;

    spinlock_acquire(&file->lock);

    CHECK_ERROR(iseof != NULL, ERROR_INVALID_PARAM);
    *iseof = file->seek == file->size;

cleanup:
    spinlock_release(&file->lock);

    return err;
}

static err_t tar_seek(file_t handle, size_t offset, seek_type_t type) {
    err_t err = NO_ERROR;
    fd_entry_t* file = (fd_entry_t*)handle;

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
    spinlock_release(&file->lock);

    return err;
}

err_t tar_tell(file_t handle, size_t* size) {
    err_t err = NO_ERROR;
    fd_entry_t* file = (fd_entry_t*)handle;

    spinlock_acquire(&file->lock);

    CHECK_ERROR(size != NULL, ERROR_NOT_FOUND);
    *size = file->seek;

cleanup:
    spinlock_release(&file->lock);

    return err;
}

err_t tar_read(file_t handle, file_io_token_t* io_token) {
    err_t err = NO_ERROR;
    fd_entry_t* file = (fd_entry_t*)handle;

    spinlock_acquire(&file->lock);

    // check if eof
    CHECK_ERROR(file->seek < file->size, ERROR_EOF);

    // truncate if needed
    if (io_token->buffer_size > file->size - file->seek) {
        io_token->buffer_size = file->size - file->seek;
    }

    // copy it
    memcpy(io_token->buffer, &file->data[file->seek], io_token->buffer_size);
    file->seek += io_token->buffer_size;

    // signal that we are done
    signal_event(io_token->event);

cleanup:
    spinlock_release(&file->lock);
    io_token->error = err;
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filesystem implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t tar_open(filesystem_t fs, const char* path, file_t* handle) {
    err_t err = NO_ERROR;

    spinlock_acquire(&lock);

    int i = shgeti(initrd_files, path);
    CHECK_ERROR(i != -1, ERROR_NOT_FOUND);
    tar_header_t* header = initrd_files[i].value;

    fd_entry_t* entry = mm_allocate(sizeof(fd_entry_t));
    entry->data = (void*)((uintptr_t)header + TAR_HEADER_SIZE);
    entry->seek = 0;
    entry->size = tar_size(header);

    // set the interface functions
    entry->file.read = tar_read;
    entry->file.close = tar_close;
    entry->file.eof = tar_eof;
    entry->file.seek = tar_seek;
    entry->file.tell = tar_tell;

   *handle = (file_t)entry;

cleanup:
    spinlock_release(&lock);
    return err;
}

static err_t tar_is_readonly(filesystem_t fs, bool* is_ro) {
    err_t err = NO_ERROR;

    *is_ro = true;

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Component implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct filesystem component = {
    .component = {
        .type = COMPONENT_FILESYSTEM,
        .address = {
            // sha1 of "initrd"
            .data1 = 0xA1B7B399, .data2 = 0xFD00, .data3 = 0x7CED,
            .data4 = { 0x7E, 0xB1, 0xC5, 0x9F, 0x4D, 0x75, 0xD8, 0x41 }
        },
        .ref_count = 1,
    },
    .open = tar_open,
    .is_readonly = tar_is_readonly,
};

void create_initrd_fs(stivale_module_t* module) {
    tar_header_t* header = (tar_header_t*)module->begin;

    // parse the initrd
    while (true) {
        // got to end of file
        if((ptrdiff_t)((uintptr_t)header + TAR_HEADER_SIZE) > (ptrdiff_t)module->end) {
            break;
        }

        // invalid name
        if (header->filename[0] == '\0') {
            break;
        }

        // make sure the file size is all good
        size_t size = ALIGN_UP(tar_size(header), TAR_HEADER_SIZE);
        ASSERT((ptrdiff_t)((uintptr_t)header + TAR_HEADER_SIZE + size) <= (ptrdiff_t)module->end);

        // add it
        shput(initrd_files, header->filename, header);

        // next
        header = (tar_header_t*)((uintptr_t)header + TAR_HEADER_SIZE + size);
    }

    // register component and set as primary for now
    register_component(&component.component);
    set_primary(&component.component);
}
