#ifndef TOMATKERNEL_IDE_H
#define TOMATKERNEL_IDE_H

/**
 * This entry is for the different ide devices connected
 */
typedef struct ata_entry {
    bool present;
    int controller;
    int port;
    size_t size_in_bytes;
    spinlock_t lock;
} ata_entry_t;

extern ata_entry_t ide_entries[4];

error_t ata_provider_init();

#endif //TOMATKERNEL_IDE_H
