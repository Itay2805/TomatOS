#include <util/debug.h>
#include <memory/mm.h>
#include <libc/string.h>

#include "object.h"

static object_t* Object_new(const void* _self, va_list ap) {
    const class_t* self = cast(Class(), _self);
    return ctor(allocate(self), ap);
}

static void* Object_ctor(void* _self, va_list ap) {
    object_t* self = cast(Object(), _self);
    return self;
}

static void* Object_dtor(void* _self) {
    struct Object* self = cast(Object(), _self);
    return self;
}

static void Object_debug_print(const void* _self) {
    const object_t* self = cast(Object(), _self);
    const class_t* class = classOf(self);

    debug_log("[*] %s at %p\n", class->name, self);
}

static void Object_delete(void* _self) {
    object_t* self = cast(Object(), _self);
    mm_pool_free(dtor(self));
}

const class_t* classOf(const void* _self) {
    const object_t* self = cast(Object(), _self);
    return self->class;
}

size_t sizeOf(const void* _self) {
    const class_t* class = classOf(_self);
    return class->size;
}

int isA(const void* _self, const void* class) {
    if (_self) {
        const object_t* self = cast(Object(), _self);
        cast(Class(), class);
        return classOf(self) == class;
    }
    return 0;
}

int isOf(const void* _self, const void* class) {
    if (_self) {
        const class_t* myClass;
        const object_t* self = cast(Object(), _self);

        cast(Class(), class);

        myClass = classOf(self);
        if (class != Object()) {
            while (myClass != class) {
                if (myClass != Object()) {
                    myClass = super(myClass);
                } else {
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

static const void* isObject(const void* p) {
    ASSERT(p != NULL);
    ASSERT(((object_t*)p)->magic == OBJECT_MAGIC);
    return p;
}

void* cast(const void* class, const void* _self) {
    const object_t* self = isObject(_self);
    const class_t* myClass = isObject(self->class);

    if (class != Object()) {
        isObject(class);
        while (myClass != class) {
            ASSERT(myClass != Object());
            myClass = myClass->super;
        }
    }

    return (void*)self;
}

static void* Class_dtor(void* _self) {
    ASSERT(0);
    return 0;
}

static void Class_delete(void* _self) {
    class_t* self = cast(Class(), _self);
    debug_log("[!] %s: cannot delete class\n", self->name);
}

object_t* allocate(const void* _self) {
    const class_t* self = cast(Class(), _self);

    ASSERT(self->size != 0);
    object_t* object = mm_pool_allocate(self->size);
    object->magic = OBJECT_MAGIC;
    object->class = self;
    return object;
}

const class_t* super(const void* _self) {
    const class_t* self = cast(Class(), _self);
    return self->super;
}

const char* nameOf(const void* _self) {
    const class_t* self = cast(Class(), _self);
    return self->name;
}

static const class_t _Object;
static const class_t _Class;

void* ctor(void* _self, va_list ap) {
    const class_t* class = classOf(_self);

    ASSERT(class->ctor != NULL);
    return class->ctor(_self, ap);
}

void* super_ctor(const void* _class, void* _self, va_list ap) {
    const class_t* superclass = super(_class);

    ASSERT(superclass->ctor != NULL);
    return superclass->ctor(_self, ap);
}

void* dtor(void* _self) {
    const class_t* class = classOf(_self);
    ASSERT(class->dtor != NULL);

    return class->dtor(_self);
}

void* super_dtor(const void* _class, void* _self) {
    const class_t* superclass = super(_class);

    ASSERT(superclass->dtor != NULL);
    return superclass->dtor(_self);
}

void debug_print(const void* _self) {
    const class_t* class = classOf(_self);

    ASSERT(class->debug_print != NULL);
    class->debug_print(_self);
}

void super_debug_print(const void* _class, const void* _self) {
    const class_t* superclass = super(_class);

    ASSERT(superclass->debug_print != NULL);
    superclass->debug_print(_self);
}

void delete (void* _self) {
    const class_t* class = classOf(_self);

    ASSERT(class->delete != NULL);
    class->delete(_self);
}

void super_delete(const void* _class, void* _self) {
    const class_t* superclass = super(_class);

    ASSERT(superclass->delete);
    superclass->delete(_self);
}

void* new(const void* _self, ...) {
    va_list ap;
    const class_t* class = cast(Class(), _self);

    va_start(ap, _self);
    ASSERT(class->new != NULL);

    object_t* result = class->new(_self, ap);
    va_end(ap);
    return result;
}

void* super_new(const void* _class, const void* _self, va_list ap) {
    const class_t* superclass = super(_class);

    ASSERT(superclass->new);
    return superclass->new(_self, ap);
}

static void* Class_ctor(void* _self, va_list ap) {
    class_t* self = _self;
    const size_t offset = offsetof(class_t, ctor);
    void* selector;

    self->name = va_arg(ap, char*);
    self->super = cast(Class(), va_arg(ap, void*));
    self->size = va_arg(ap, size_t);

    memcpy((char*)self + offset, (char*)self->super + offset, sizeOf(self->super) - offset);

    va_list lap;
    va_copy(lap, ap);
    while ((selector = va_arg(lap, void*))) {
        void* method = va_arg(lap, void*);

        if (selector == (void*)ctor) {
            self->ctor = method;
            continue;
        }
        if (selector == (void*)dtor) {
            self->dtor = method;
            continue;
        }
        if (selector == (void*)debug_print) {
            self->debug_print = method;
            continue;
        }
        if (selector == (void*) delete) {
            self->delete = method;
            continue;
        }
        if (selector == (void*) new) {
            self->new = method;
            continue;
        }
    }
    return self;
}

const void* Object(void) {
    return &_Object;
}

const void* Class(void) {
    return &_Class;
}

static const class_t _Object = {
        { OBJECT_MAGIC, &_Class },
        "Object", &_Object, sizeof(object_t),
        Object_ctor,
        Object_dtor,
        Object_debug_print,
        Object_delete,
        Object_new,
};

static const class_t _Class = {
        { OBJECT_MAGIC, &_Class },
        "Class", &_Object, sizeof(class_t),
        Class_ctor,
        Class_dtor,
        Object_debug_print,
        Class_delete,
        Object_new,
};
