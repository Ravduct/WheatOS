#ifndef FS_H
#define FS_H

#include <stddef.h>

#define MAX_FILES 16
#define NAME_SIZE 32
#define DATA_SIZE 512

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
    if (file == NULL || data == NULL || size > DATA_SIZE) {
        return -1; // Invalid parameters
    }

    for (size_t i = 0; i < size; i++) {
        file->data[i] = data[i];
    }
    file->size = size;
    return 0; // Success
}

#endif // FS_H