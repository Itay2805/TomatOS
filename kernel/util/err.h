#ifndef TOMATOS_ERR_H
#define TOMATOS_ERR_H

typedef struct err {
    uint32_t value;
} err_t;

#define NO_ERROR                    ((err_t){ 0 })
#define ERROR_CHECK_FAILED          ((err_t){ 1 })
#define ERROR_NOT_FOUND             ((err_t){ 2 })
#define ERROR_OUT_OF_RESOURCES      ((err_t){ 3 })

#define IS_ERROR(err) ((err).value != 0)

#endif //TOMATOS_ERR_H
