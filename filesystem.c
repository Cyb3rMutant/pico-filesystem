#include "filesystem.h"
#include "flash_ops.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

char temp_buffer[4096];
void clear_buffer() { memset(temp_buffer, 0, sizeof(temp_buffer)); }

FileEntry file_table[25];

FS_FILE open_files[10];

int get_file(const char *path) {
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, path) == 0) {
            return i;
        }
    }
    return FILE_NOT_FOUND;
}
int get_fd() {
    for (int i = 0; i < 10; i++) {
        if (open_files[i].entry == NULL) {
            return i;
        }
    }
    return OPENED_FILES_FULL;
}

// Function to check if a mode is set
int check_mode(int mode, int flag) {
    return mode & flag; // Check if the flag is set
}
void update_file_table() {
    flash_write_safe(0, &file_table, sizeof(FileEntry) * 25);
}

#define FLASH_TARGET_OFFSET                                                    \
    (256 * 1024) // Offset where user data starts (256KB into flash)

void init_filesystem() {
    // Initialize the file table
    uint32_t flash_address = FLASH_TARGET_OFFSET;
    flash_read_safe(0, &file_table, sizeof(FileEntry) * 25);

    if (strcmp(file_table[0].filename, "magic string for initing\0") == 0) {
        return;
    }

    memcpy(file_table[0].filename, "magic string for initing\0", 25);
    file_table[0].size = 0;
    file_table[0].in_use = 0;
    for (int i = 1; i < 25; i++) {
        file_table[i].filename[0] = '\0';
        file_table[i].size = 0;
        file_table[i].in_use = 0;
    }

    update_file_table();
}

/**
 * Opens a file with the specified path and mode.
 *
 * @param path The path of the file to be opened.
 * @param m The mode in which the file should be opened.
 * @return A file descriptor, or an error code if an error occurred.
 */
int fs_open(const char *path, int m) {
    if (check_mode(m, MODE_READ) && check_mode(m, MODE_APPEND)) {
        return INCORRECT_MODE;
    }
    int file = get_file(path);
    if (file == FILE_NOT_FOUND && check_mode(m, MODE_CREATE)) {
        file = fs_create(path);
    }
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    if (file_table[file].in_use == 1) {
        return FILE_ALREADY_OPEN;
    }
    int fd = get_fd();
    if (fd == OPENED_FILES_FULL) {
        return OPENED_FILES_FULL;
    }
    file_table[file].in_use = 1;
    open_files[fd].entry = &file_table[file];
    open_files[fd].m = m;
    open_files[fd].position = 0;
    return fd;
}

/**
 * Closes the specified file.
 *
 * @param fd A file descriptor to an opened file.
 */
void fs_close(int fd) {
    open_files[fd].entry->in_use = 0;
    open_files[fd].m = 0;
    open_files[fd].position = 0;
    open_files[fd].entry = NULL;
}

/**
 * Reads data from the specified file into the provided buffer.
 *
 * @param fd A file descriptor to an opened file.
 * @param buffer A pointer to the buffer where the read data will be stored.
 * @param size   The maximum number of bytes to read.
 * @return The number of bytes read, or -1 if an error occurred.
 */
int fs_read(int fd, char *buffer, int size) {

    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }

    if (!check_mode(open_files[fd].m, MODE_READ)) {
        return INCORRECT_MODE;
    }
    if (open_files[fd].entry->size == 0) {
        return 0;
    }
    if (open_files[fd].position + size > open_files[fd].entry->size) {
        size = open_files[fd].entry->size - open_files[fd].position;
    }

    clear_buffer();
    flash_read_safe(get_file(open_files[fd].entry->filename), temp_buffer,
                    size);
    memcpy(buffer, temp_buffer + open_files[fd].position, size);
    open_files[fd].position += size;
    return size;
}

int write_helper(int fd, const char *buffer, int size) {
    // Check if the position and size exceed the buffer
    if (open_files[fd].position + size > sizeof(temp_buffer)) {
        // Handle buffer overflow
        return OVERFLOW; // or any appropriate error code
    }

    // Read existing data into temp_buffer
    clear_buffer();
    flash_read_safe(get_file(open_files[fd].entry->filename), temp_buffer,
                    open_files[fd].entry->size);

    // Copy new data into temp_buffer at the appropriate position
    memcpy(temp_buffer + open_files[fd].position, buffer, size);

    // Update the position
    open_files[fd].position += size;

    // Update the file size
    if (open_files[fd].entry->size < open_files[fd].position) {
        open_files[fd].entry->size = open_files[fd].position;
    }

    // Write back to the file
    flash_write_safe(get_file(open_files[fd].entry->filename), temp_buffer,
                     open_files[fd].entry->size);

    // Return the size of data copied
    return size;
}
/**
 * Writes data from the provided buffer to the specified file.
 *
 * @param fd A file descriptor to an opened file.
 * @param buffer A pointer to the buffer containing the data to be written.
 * @param size   The number of bytes to write.
 * @return The number of bytes written, or -1 if an error occurred.
 */
int fs_write(int fd, const char *buffer, int size) {
    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }
    int t_size = size;
    if (check_mode(open_files[fd].m, MODE_WRITE)) {
        t_size = write_helper(fd, buffer, size);
    } else if (check_mode(open_files[fd].m, MODE_APPEND)) {
        int cur_pos = open_files[fd].position;
        fs_seek(fd, 0, SEEK_END);
        t_size = write_helper(fd, buffer, size);
        fs_seek(fd, cur_pos, SEEK_SET);
    } else {
        return INCORRECT_MODE;
    }

    return t_size;
}

/**
 * Sets the file position indicator for the specified file.
 *
 * @param file   A pointer to the file to seek in.
 * @param offset The number of bytes to offset from the specified position.
 * @param whence The position from which to calculate the offset.
 * @return 0 if successful, or -1 if an error occurred.
 */
int fs_seek(int fd, long offset, int whence) {
    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }
    switch (whence) {
    case SEEK_SET:
        open_files[fd].position = offset;
        break;
    case SEEK_CUR:
        open_files[fd].position += offset;
        break;
    case SEEK_END:
        open_files[fd].position = open_files[fd].entry->size;
        break;
    }
    return open_files[fd].position;
}

int fs_create(const char *path) {
    if (get_file(path) != FILE_NOT_FOUND) {
        return FILE_ALREADY_EXISTS;
    }
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, "\0") == 0) {
            strcpy(file_table[i].filename, path);
            file_table[i].size = 0;
            file_table[i].in_use = 0;
            update_file_table();
            return i;
        }
    }
    return FILE_TABLE_FULL;
}
void fs_ls() {
    printf("\nfilename size in_use\n");
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, "\0") == 0) {
            continue;
        }
        printf("%s %d %d\n", file_table[i].filename, file_table[i].size,
               file_table[i].in_use);
    }
}

int fs_format(const char *path) {
    int file = get_file(path);
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    file_table[file].size = 0;
    update_file_table();
    flash_erase_safe(file);
}

void fs_wipe() {
    for (int i = 1; i < 25; i++) {
        file_table[i].filename[0] = '\0';
        file_table[i].size = 0;
        file_table[i].in_use = 0;
        flash_erase_safe(i);
    }
    update_file_table();
}

int fs_mv(const char *old_path, const char *new_path) {
    int old_file = get_file(old_path);
    if (old_file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    int new_file = get_file(new_path);
    if (new_file == FILE_NOT_FOUND) {
        strcpy(file_table[old_file].filename, new_path);
        update_file_table();
    } else {
        fs_cp(old_path, new_path);
        fs_rm(old_path);
    }

    return 0;
}

int fs_cp(const char *source_path, const char *dest_path) {
    int source = get_file(source_path);
    int dest = get_file(dest_path);
    if (source == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    if (dest == FILE_NOT_FOUND) {
        dest = fs_create(dest_path);
    }
    file_table[dest].size = file_table[source].size;
    update_file_table();
    clear_buffer();
    flash_read_safe(source, temp_buffer, file_table[source].size);
    flash_write_safe(dest, temp_buffer, file_table[dest].size);
    return 0;
}

int fs_rm(const char *path) {
    int file = get_file(path);
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    file_table[file].filename[0] = '\0';
    file_table[file].size = 0;
    file_table[file].in_use = 0;
    update_file_table();
    flash_erase_safe(file);
    return 0;
}
