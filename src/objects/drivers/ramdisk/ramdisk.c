#include <objects/object.h>
#include <common/common.h>
#include <stb/stb_ds.h>
#include <stdlib.h>
#include <objects/storage.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include "ramdisk.h"

static const char* ramdisk_name = "ramdisk";

static error_t ramdisk_read(object_t* obj, void* buffer, size_t length, size_t offset) {
    error_t err = NO_ERROR;

    CHECK(obj != NULL);
    CHECK(obj->name == ramdisk_name);

    CHECK(buffer != NULL);
    CHECK(length > 0);

    ramdisk_t* ramdisk = obj->context;
    CHECK(ramdisk != NULL);

    // make sure we don't read out of the buffer
    CHECK(length + offset < arrlen(ramdisk->sectors) * KB(4));

    // in this case we have one block, we can do a simply memcpy
    if(ALIGN_PAGE_DOWN(offset) == ALIGN_PAGE_DOWN(length + offset)) {
        size_t offset_into_first = offset - ALIGN_PAGE_DOWN(offset);
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(buffer, ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] + offset_into_first, length);

        // need to read two or more sectors
    }else {
        // first sector we take the offset to account
        size_t read_from_first = ALIGN_PAGE_UP(offset) - offset;
        size_t offset_into_first = KB(4) - read_from_first;
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(buffer, ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] + offset_into_first, read_from_first);
        length -= read_from_first;
        buffer += read_from_first;
        offset += read_from_first;

        // now read the middle sectors normally
        size_t pages_to_read = ALIGN_PAGE_DOWN(length) / KB(4);
        size_t second_sector = ALIGN_PAGE_DOWN(offset) / KB(4);
        for(size_t i = second_sector; i < second_sector + pages_to_read; i++) {
            CHECK(ramdisk->sectors[i] != NULL);
            memcpy(buffer, ramdisk->sectors[i], KB(4));
            buffer += KB(4);
            offset += KB(4);
            length -= KB(4);
        }

        // now read the last sector
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(buffer, ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)], length);
    }

    cleanup:
    return err;
}

static error_t ramdisk_write(object_t* obj, void* buffer, size_t length, size_t offset) {
    error_t err = NO_ERROR;

    CHECK(obj != NULL);
    CHECK(obj->name == ramdisk_name);

    CHECK(buffer != NULL);
    CHECK(length > 0);

    ramdisk_t* ramdisk = obj->context;
    CHECK(ramdisk != NULL);

    // make sure we don't write out of the buffer
    CHECK(length + offset < arrlen(ramdisk->sectors) * KB(4));

    // in this case we have one block, we can do a simply memcpy
    if(ALIGN_PAGE_DOWN(offset) == ALIGN_PAGE_DOWN(length + offset)) {
        size_t offset_into_first = offset - ALIGN_PAGE_DOWN(offset);
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] + offset_into_first, buffer, length);

    // need to write two or more sectors
    }else {
        // first sector we take the offset to account
        size_t read_from_first = ALIGN_PAGE_UP(offset) - offset;
        size_t offset_into_first = KB(4) - read_from_first;
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] + offset_into_first, buffer, read_from_first);
        length -= read_from_first;
        buffer += read_from_first;
        offset += read_from_first;

        // now write the middle sectors normally
        size_t pages_to_read = ALIGN_PAGE_DOWN(length) / KB(4);
        size_t second_sector = ALIGN_PAGE_DOWN(offset) / KB(4);
        for(size_t i = second_sector; i < second_sector + pages_to_read; i++) {
            CHECK(ramdisk->sectors[i] != NULL);
            memcpy(ramdisk->sectors[i], buffer, KB(4));
            buffer += KB(4);
            offset += KB(4);
            length -= KB(4);
        }

        // now write the last sector
        CHECK(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)] != NULL);
        memcpy(ramdisk->sectors[ALIGN_PAGE_DOWN(offset) / KB(4)], buffer, length);
    }

cleanup:
    return err;
}

static error_t ramdisk_get_sector_size(object_t* obj, size_t* buffer) {
    error_t err = NO_ERROR;

    CHECK(obj != NULL);
    CHECK(obj->name == ramdisk_name);

    *buffer = KB(4);

cleanup:
    return err;
}

static storage_functions_t ramdisk_functions = {
    .read = ramdisk_read,
    .write = ramdisk_write,
    .get_sector_size = ramdisk_get_sector_size,
};

error_t ramdisk_create(size_t size) {
    error_t err = NO_ERROR;

    CHECK_TRACE(size % KB(4) == 0, "ramdisk size must be 4k aligned");

    ramdisk_t* ramdisk = kmalloc(sizeof(ramdisk_t));
    CHECK(ramdisk != NULL);

    arrsetlen(ramdisk->sectors, size / KB(4));
    for(int i = 0; i < size / KB(4); i++) {
        CHECK_AND_RETHROW(pmm_allocate((uintptr_t*)&ramdisk->sectors[i]));
        ramdisk->sectors[i] += DIRECT_MAPPING_BASE;
    }

    // create the object
    object_t* obj = kmalloc(sizeof(object_t));
    CHECK(obj != NULL);

    obj->type = OBJECT_STORAGE;
    obj->name = ramdisk_name;
    obj->context = ramdisk;
    obj->functions = &ramdisk_functions;
    // TODO: syscalls
    CHECK_AND_RETHROW(object_add(obj));

cleanup:
    if(err != NO_ERROR) {
        /*
         * If we failed free everything we need to free
         *
         * TODO: Need a way to remove the obj
         */
        if(ramdisk != NULL) {

            if(ramdisk->sectors != NULL) {

                for(int i = 0; i < size / KB(4); i++) {
                    if(ramdisk->sectors[i] != 0) {
                        CATCH(pmm_free((uintptr_t)ramdisk->sectors[i] - DIRECT_MAPPING_BASE));
                    }
                }

                arrfree(ramdisk->sectors);
            }

            kfree(ramdisk);
        }

        if(obj != NULL) {
            // TODO: remove from the objects!!
            kfree(obj);
        }
    }
    return err;
}
