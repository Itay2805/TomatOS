#include <util/defs.h>
#include "filesystem.h"

lock_t filesystems_lock;
list_entry_t filesystems = INIT_LIST_ENTRY(filesystems);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File system class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* FileSystemClass_ctor(void* _self, va_list ap) {
    filesystem_class_t* self = super_ctor(FileSystemClass(), _self, ap);
    void* selector;

    va_list lap;
    va_copy(lap, ap);
    while((selector = va_arg(lap, void*)) != NULL) {
        void* method = va_arg(lap, void*);

        if(selector == fs_open) {
            self->open = method;
        }else if(selector == fs_stat) {
            self->open = method;
        }
    }

    return self;
}

const void* FileSystemClass() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(Class(),
                    "FileSystemClass", Class(), sizeof(filesystem_class_t),
                    ctor, FileSystemClass_ctor,
                    0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* FileClass_ctor(void* _self, va_list ap) {
    file_class_t* self = super_ctor(FileClass(), _self, ap);
    void* selector;

    va_list lap;
    va_copy(lap, ap);
    while((selector = va_arg(lap, void*)) != NULL) {
        void* method = va_arg(lap, void*);

        if(selector == file_close) {
            self->close = method;
        }else if(selector == file_read) {
            self->read = method;
        }
    }

    return self;
}

const void* FileClass() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(Class(),
                "FileClass", Class(), sizeof(file_class_t),
                ctor, FileClass_ctor,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File system implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* FileSystem_ctor(void* _self, va_list ap) {
    filesystem_t* self = super_ctor(FileSystem(), _self, ap);

    self->files = INIT_LIST_ENTRY(self->files);

    return self;
}

static void* FileSystem_dtor(void* _self) {
    filesystem_t* self = super_dtor(FileSystem(), _self);

    ASSERT(is_list_empty(&self->files));

    return self;
}

const void* FileSystem() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(FileSystemClass(),
                "FileSystem", Object(), sizeof(filesystem_t),
                ctor, FileSystem_ctor,
                dtor, FileSystem_dtor,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* File_ctor(void* _self, va_list ap) {
    file_t* self = super_ctor(File(), _self, ap);

    self->parent = va_arg(ap, filesystem_t*);

    return self;
}

const void* File() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(FileClass(),
                "File", Object(), sizeof(file_t),
                ctor, File_ctor,
                0);
    }
    return class;
}
