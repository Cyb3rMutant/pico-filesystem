#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_FILES 10 // Maximum number of files in the filesystem

#define MODE_READ (1 << 0)   // 0001
#define MODE_WRITE (1 << 1)  // 0010
#define MODE_APPEND (1 << 2) // 0100
#define MODE_CREATE (1 << 3) // 1000

enum whence { FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END };

enum errors {
    FILE_NOT_FOUND = -1,
    FILE_ALREADY_EXISTS = -2,
    FILE_TABLE_FULL = -3,
    FILE_NOT_OPEN = -4,
    INCORRECT_MODE = -5,
    FILE_ALREADY_OPEN = -6,
    OPENED_FILES_FULL = -7,
    OVERFLOW = -8,
};

// File entry structure
typedef struct {
    char filename[25]; // Path of the file
    uint32_t size;     // Size of the file
    bool in_use;       // Indicates if this file entry is in use
} FileEntry;

// File handle structure
typedef struct {
    FileEntry *entry;  // Pointer to the file entry in the file system
    uint32_t position; // Current position in the file
    int m;
} FS_FILE;

int check_mode(int mode, int flag);

void init_filesystem();
int fs_open(const char *path, int m);
void fs_close(int fd);
int fs_read(int fd, char *buffer, int size);
int fs_write(int fd, const char *buffer, int size);
int fs_seek(int fd, long offset, int whence);

int fs_create(const char *path);
void fs_ls();
int fs_format(const char *path);
void fs_wipe();
int fs_mv(const char *old_path, const char *new_path);
int fs_cp(const char *source_path, const char *dest_path);
int fs_rm(const char *path);

#endif // FILESYSTEM_H
