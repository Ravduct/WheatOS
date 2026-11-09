#ifndef FS_H
#define FS_H

#include <stddef.h>
#include "ata.h"

#define MAX_FILES 16
#define NAME_SIZE 32
#define DATA_SIZE 512

#define FS_MAGIC 0x57484541
#define FS_SECTORS ((sizeof(files) + 511) / 512)

typedef struct {
    char name[NAME_SIZE];
    char data[DATA_SIZE];
    size_t size;
    int used;
} File;

static File files[MAX_FILES];

static int same_name(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

File *fs_find(const char *name) {
    if (name == NULL) return NULL;

    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && same_name(files[i].name, name)) {
            return &files[i];
        }
    }
    return NULL;
}

File *fs_create(const char *name) {
    if (name == NULL || name[0] == '\0' || fs_find(name) != NULL) {
        return NULL;
    }

    size_t length = 0;
    while (name[length] != '\0') {
        if (length >= NAME_SIZE - 1) return NULL;
        length++;
    }

    for (size_t i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            for (size_t j = 0; j <= length; j++) {
                files[i].name[j] = name[j];
            }
            files[i].data[0] = '\0';
            files[i].size = 0;
            files[i].used = 1;
            return &files[i];
        }
    }

    return NULL; // Table is full.
}

int fs_write(File *file, const char *data, size_t size) {
    if (file == NULL || data == NULL || size >= DATA_SIZE) {
        return -1; // Invalid parameters
    }

    for (size_t i = 0; i < size; i++) {
        file->data[i] = data[i];
    }
    file->data[size] = '\0';
    file->size = size;
    return 0; // Success
}

int fs_remove(const char *name) {
    File *file = fs_find(name);
    if (file == NULL) return -1;

    file->name[0] = '\0';
    file->data[0] = '\0';
    file->size = 0;
    file->used = 0;
    return 0;
}

int fs_save(void) {
    uint8_t buf[512];

    for (int i = 0; i < 512; i++) buf[i] = 0;
    *(uint32_t *)buf = FS_MAGIC;
    if (ata_write_sector(0, buf) != 0) return -1;

    const uint8_t *src = (const uint8_t *)files;
    for (size_t s = 0; s < FS_SECTORS; s++) {
        size_t offset = s * 512;
        size_t n = sizeof(files) - offset;
        if (n > 512) n = 512;

        for (size_t i = 0; i < 512; i++) buf[i] = (i < n) ? src[offset + i] : 0;
        if (ata_write_sector(1 + s, buf) != 0) return -1;
    }
    return 0;
}

int fs_load(void) {
    uint8_t buf[512];

    if (ata_read_sector(0, buf) != 0 || *(uint32_t *)buf != FS_MAGIC) {
        uint8_t *dst = (uint8_t *)files;
        for (size_t i = 0; i < sizeof(files); i++) dst[i] = 0;
        return fs_save();      // blank disk: format it
    }

    uint8_t *dst = (uint8_t *)files;
    for (size_t s = 0; s < FS_SECTORS; s++) {
        size_t offset = s * 512;
        size_t n = sizeof(files) - offset;
        if (n > 512) n = 512;

        if (ata_read_sector(1 + s, buf) != 0) return -1;
        for (size_t i = 0; i < n; i++) dst[offset + i] = buf[i];
    }
    return 0;
}

#endif // FS_H