#ifndef TOMATKERNEL_OBJECT_H
#define TOMATKERNEL_OBJECT_H

#include <stddef.h>
#include <stdarg.h>

#define	OBJECT_MAGIC 0xCAFEBABE

typedef struct object {
    unsigned long magic;
    const void* class;
} object_t;

typedef struct class {
    object_t _;
    const char* name;
    const void* super;
    size_t size;

    void* (*ctor)(void* _self, va_list ap);
    void* (*dtor)(void* _self);
    void (*debug_print)();
    void (*delete)(void* _self);
    object_t* (*new)(const void* _self, va_list ap);
} class_t;

void* super_ctor(const void* _class, void* _self, va_list ap);
void* super_dtor(const void* _class, void* _self);
void super_debug_print(const void* _class, const void* _self);
void super_delete(const void* _class, void* _self);

void* super_new(const void* _class, const void* _self, va_list ap);

const void* Object();

const class_t* classOf(const void* _self);
size_t sizeOf(const void* _self);
int isA(const void* _self, const void* class);
int isOf(const void* _self, const void* class);
void* cast(const void* class, const void* _self);

void* ctor(void* _self, va_list ap);
void* dtor(void* _self);
void debug_print(const void* _self);
void delete (void* _self);

void* new(const void* _self, ...);

const void* Class();

object_t* allocate(const void* _self);
const class_t* super(const void* _self);
const char* nameOf(const void* _self);

#endif //TOMATKERNEL_OBJECT_H
