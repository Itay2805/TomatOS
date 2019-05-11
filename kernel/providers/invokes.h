#ifndef TOMATKERNEL_INVOKES_H
#define TOMATKERNEL_INVOKES_H

/*
 * Standard invokes that should be implemented on any file system provider
 *
 */

#define FS_READ_DIR         0x80000000
#define FS_READ_DIR_RESET   0x80000001

typedef struct dir_entry {
    char name[256];
    int type;
} dir_entry_t;

#define FS_RESOURCE_TYPE    0x80000002
#define FS_FILE             0
#define FS_DIR              1

#endif //TOMATKERNEL_INVOKES_H
