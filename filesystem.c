#include "filesystem.h"
#include "flash_ops.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

char temp_buffer[4096];

FileEntry file_table[25];

FS_FILE open_files[10];

/**
 * @brief Clears the temporary buffer.
 */
void clear_buffer() { memset(temp_buffer, 0, sizeof(temp_buffer)); }

/**
 * @brief Searches for a file in the file table by its path.
 *
 * @param path The path of the file to search for.
 * @return The index of the file entry if found, otherwise FILE_NOT_FOUND.
 */
int get_file(const char *path) {
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, path) == 0) {
            return i;
        }
    }
    return FILE_NOT_FOUND;
}

/**
 * @brief Retrieves an available file descriptor.
 *
 * @return The index of an available file descriptor if found, otherwise
 * OPENED_FILES_FULL.
 */
int get_fd() {
    for (int i = 0; i < 10; i++) {
        if (open_files[i].entry == NULL) {
            return i;
        }
    }
    return OPENED_FILES_FULL;
}

/**
 * @brief Checks if a specific mode is set.
 *
 * @param mode The mode to check.
 * @param flag The flag to check for.
 * @return 1 if the flag is set in the mode, otherwise 0.
 */
int check_mode(int mode, int flag) {
    return mode & flag; // Check if the flag is set
}

/**
 * @brief Updates the file table in the flash memory.
 */
void update_file_table() {
    flash_write_safe(0, &file_table, sizeof(FileEntry) * 25);
}

/**
 * @brief Initializes the filesystem.
 *
 * This function initializes the file table and checks if the filesystem has
 * already been initialized. If the filesystem has not been initialized, it
 * initializes the file table with a magic string and default values.
 */
void init_filesystem() {
    // Initialize the file table
    flash_read_safe(0, &file_table, sizeof(FileEntry) * 25);
    for (int i = 1; i < 25; i++) {
        file_table[i].in_use = 0;
    }

    if (strcmp(file_table[0].filename, "magic string for initing\0") == 0) {
        return;
    }

    // Initialize the first entry with a magic string and default values
    memcpy(file_table[0].filename, "magic string for initing\0", 25);
    file_table[0].size = 0;
    file_table[0].in_use = 0;

    // Initialize other entries with default values
    for (int i = 1; i < 25; i++) {
        file_table[i].filename[0] = '\0';
        file_table[i].size = 0;
        file_table[i].in_use = 0;
    }

    // Update the file table in flash memory
    update_file_table();
}

/**
 * @brief Opens a file with the specified path and mode.
 *
 * This function opens a file with the specified path and mode. If the file does
 * not exist and the mode allows, it creates the file. Returns a file descriptor
 * if successful, otherwise returns an error code.
 *
 * @param path The path of the file to open.
 * @param m The mode for opening the file.
 * @return A file descriptor if successful, otherwise an error code.
 */
int fs_open(const char *path, int m) {
    // Check if both read and append modes are set
    if (check_mode(m, MODE_WRITE) && check_mode(m, MODE_APPEND)) {
        return INCORRECT_MODE;
    }

    // Check if the file exists, and create if necessary
    int file = get_file(path);
    if (file == FILE_NOT_FOUND && check_mode(m, MODE_CREATE)) {
        file = fs_create(path);
    }

    // Return error if file not found
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }

    // Return error if file is already open
    if (file_table[file].in_use == 1) {
        return FILE_ALREADY_OPEN;
    }

    // Get an available file descriptor
    int fd = get_fd();
    if (fd == OPENED_FILES_FULL) {
        return OPENED_FILES_FULL;
    }

    // Update file table and open_files with file information
    file_table[file].in_use = 1;
    open_files[fd].entry = &file_table[file];
    open_files[fd].m = m;
    open_files[fd].position = 0;
    return fd;
}

/**
 * @brief Closes the file associated with the given file descriptor.
 *
 * @param fd The file descriptor of the file to close.
 */
void fs_close(int fd) {
    open_files[fd].entry->in_use = 0;
    open_files[fd].m = 0;
    open_files[fd].position = 0;
    open_files[fd].entry = NULL;
}

/**
 * @brief Reads data from the file associated with the given file descriptor
 * into the buffer.
 *
 * This function reads data from the file associated with the given file
 * descriptor into the buffer. Returns the number of bytes read if successful,
 * otherwise returns an error code.
 *
 * @param fd The file descriptor of the file to read from.
 * @param buffer The buffer to store the read data.
 * @param size The maximum number of bytes to read.
 * @return The number of bytes read if successful, otherwise an error code.
 */
int fs_read(int fd, char *buffer, int size) {
    // Return error if file not open
    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }

    // Return error if read mode not set
    if (!check_mode(open_files[fd].m, MODE_READ)) {
        return INCORRECT_MODE;
    }

    // Return 0 if file size is 0
    if (open_files[fd].entry->size == 0) {
        return 0;
    }

    // Adjust size if reading beyond file size
    if (open_files[fd].position + size > open_files[fd].entry->size) {
        size = open_files[fd].entry->size - open_files[fd].position;
    }

    // Read data from flash memory into buffer
    clear_buffer();
    flash_read_safe(get_file(open_files[fd].entry->filename), temp_buffer,
                    open_files[fd].entry->size);
    memcpy(buffer, temp_buffer + open_files[fd].position, size);
    open_files[fd].position += size;
    return size;
}

/**
 * @brief Helper function to write data from the buffer to the file.
 *
 * This function writes data from the buffer to the file associated with the
 * given file descriptor. Returns the number of bytes written if successful,
 * otherwise returns an error code.
 *
 * @param fd The file descriptor of the file to write to.
 * @param buffer The buffer containing the data to write.
 * @param size The number of bytes to write.
 * @return The number of bytes written if successful, otherwise an error code.
 */
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
 * @brief Writes data from the buffer to the file associated with the given file
 * descriptor.
 *
 * This function writes data from the buffer to the file associated with the
 * given file descriptor. Returns the number of bytes written if successful,
 * otherwise returns an error code.
 *
 * @param fd The file descriptor of the file to write to.
 * @param buffer The buffer containing the data to write.
 * @param size The number of bytes to write.
 * @return The number of bytes written if successful, otherwise an error code.
 */
int fs_write(int fd, const char *buffer, int size) {
    // Return error if file not open
    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }

    int t_size = size;
    if (check_mode(open_files[fd].m, MODE_WRITE)) {
        // Write data using helper function for MODE_WRITE
        t_size = write_helper(fd, buffer, size);
    } else if (check_mode(open_files[fd].m, MODE_APPEND)) {
        // Save current position, move to end of file, write data, then move
        // back to original position
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
 * @brief Seeks to a specified position in the file associated with the given
 * file descriptor.
 *
 * This function sets the current position in the file associated with the given
 * file descriptor. Returns the new position if successful, otherwise returns an
 * error code.
 *
 * @param fd The file descriptor of the file to seek in.
 * @param offset The offset from the origin specified by whence.
 * @param whence The reference point for the offset.
 * @return The new position if successful, otherwise an error code.
 */
int fs_seek(int fd, long offset, int whence) {
    // Return error if file not open
    if (open_files[fd].entry->in_use == 0) {
        return FILE_NOT_OPEN;
    }

    // Perform seek operation based on whence
    switch (whence) {
    case SEEK_SET:
        open_files[fd].position = offset;
        break;
    case SEEK_CUR:
        open_files[fd].position += offset;
        break;
    case SEEK_END:
        open_files[fd].position = open_files[fd].entry->size - offset;
        break;
    }

    return open_files[fd].position;
}

/**
 * @brief Creates a new file with the specified path.
 *
 * This function creates a new file with the specified path if it does not
 * already exist. Returns the index of the newly created file if successful,
 * otherwise returns an error code.
 *
 * @param path The path of the file to create.
 * @return The index of the newly created file if successful, otherwise an error
 * code.
 */
int fs_create(const char *path) {
    // Check if file already exists
    if (get_file(path) != FILE_NOT_FOUND) {
        return FILE_ALREADY_EXISTS;
    }

    // Find an empty slot in the file table and create the file
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, "\0") == 0) {
            strcpy(file_table[i].filename, path);
            file_table[i].size = 0;
            file_table[i].in_use = 0;
            update_file_table();
            return i;
        }
    }

    // Return error if file table is full
    return FILE_TABLE_FULL;
}

/**
 * @brief Lists all files in the filesystem along with their attributes.
 * @return The number of files in the filesystem.
 */
int fs_ls() {
    int count = 0;
    printf("\nfilename size in_use\n");
    for (int i = 1; i < 25; i++) {
        if (strcmp(file_table[i].filename, "\0") == 0) {
            continue;
        }
        printf("%s %d %d\n", file_table[i].filename, file_table[i].size,
               file_table[i].in_use);
        count++;
    }
    return count;
}

/**
 * @brief Formats the file with the specified path.
 *
 * This function formats the file with the specified path, resetting its size to
 * 0.
 *
 * @param path The path of the file to format.
 * @return FILE_NOT_FOUND if the file does not exist, otherwise no return value.
 */
int fs_format(const char *path) {
    // Find the file and reset its size to 0
    int file = get_file(path);
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }
    file_table[file].size = 0;
    update_file_table();
    flash_erase_safe(file);
}

/**
 * @brief Wipes all files and resets the filesystem.
 *
 * This function wipes all files and resets the filesystem, clearing all data
 * and erasing flash memory.
 */
void fs_wipe() {
    // Clear file table and erase flash memory for each file
    for (int i = 1; i < 25; i++) {
        file_table[i].filename[0] = '\0';
        file_table[i].size = 0;
        file_table[i].in_use = 0;
        flash_erase_safe(i);
    }
    update_file_table();
}

/**
 * @brief Moves a file from the old path to the new path.
 *
 * This function moves a file from the old path to the new path. If the file at
 * the new path already exists, it overwrites the existing file with the content
 * of the file at the old path.
 *
 * @param old_path The old path of the file to move.
 * @param new_path The new path of the file.
 * @return FILE_NOT_FOUND if the file at the old path does not exist, otherwise
 * no return value.
 */
int fs_mv(const char *old_path, const char *new_path) {
    // Get the index of the file at the old path
    int old_file = get_file(old_path);
    if (old_file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }

    // Check if a file exists at the new path
    int new_file = get_file(new_path);
    if (new_file == FILE_NOT_FOUND) {
        // Update the filename and update the file table if no file exists at
        // the new path
        strcpy(file_table[old_file].filename, new_path);
        update_file_table();
    } else {
        // Copy the file from the old path to the new path and then remove the
        // file at the old path
        fs_cp(old_path, new_path);
        file_table[old_file].in_use = file_table[new_file].in_use;
        if (strcmp(old_path, new_path) != 0) {
            fs_rm(old_path);
        }
    }

    return 0;
}

/**
 * @brief Copies a file from the source path to the destination path.
 *
 * This function copies a file from the source path to the destination path.
 * If the destination file already exists, it overwrites the existing file.
 *
 * @param source_path The path of the source file to copy.
 * @param dest_path The path of the destination file.
 * @return FILE_NOT_FOUND if the source file does not exist, otherwise no return
 * value.
 */
int fs_cp(const char *source_path, const char *dest_path) {
    // Get the index of the source file
    int source = get_file(source_path);
    int dest = get_file(dest_path);

    // Check if the source file exists
    if (source == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }

    // Create or get the index of the destination file
    if (dest == FILE_NOT_FOUND) {
        dest = fs_create(dest_path);
    }

    // Copy the size and content of the source file to the destination file
    file_table[dest].size = file_table[source].size;
    update_file_table();
    clear_buffer();
    flash_read_safe(source, temp_buffer, file_table[source].size);
    flash_write_safe(dest, temp_buffer, file_table[dest].size);
    return 0;
}

/**
 * @brief Removes the file at the specified path.
 *
 * This function removes the file at the specified path from the filesystem.
 *
 * @param path The path of the file to remove.
 * @return FILE_NOT_FOUND if the file does not exist, otherwise no return value.
 */
int fs_rm(const char *path) {
    // Get the index of the file to remove
    int file = get_file(path);
    if (file == FILE_NOT_FOUND) {
        return FILE_NOT_FOUND;
    }

    // Clear the filename, size, and in_use flag of the file, erase flash memory
    // for the file, and update the file table
    file_table[file].filename[0] = '\0';
    file_table[file].size = 0;
    file_table[file].in_use = 0;
    update_file_table();
    flash_erase_safe(file);
    return 0;
}
