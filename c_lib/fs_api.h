#ifndef FS_API_H
#define FS_API_H

#define MAX_FILES 16
#define NAME_SIZE 32
#define DATA_SIZE 512

typedef struct {
    const char *name;
    const char *text;
} fs_write_request;

#endif
