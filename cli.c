#include "cli.h"
#include "custom_fgets.h"
#include "filesystem.h"
#include "flash_ops.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Executes a command based on the input string.
 *
 * This function parses the input command string and executes the corresponding
 * command. It tokenizes the command string to identify the command keyword and
 * calls the appropriate handler function for that command.
 *
 * Commands:
 *  1. open: <filename> <mode> - Opens a file with the specified filename and
 * mode.
 *  2. close: <fd> - Closes the file with the specified file descriptor.
 *  3. read: <fd> <size> - Reads data from the file associated with the
 * specified file descriptor.
 *  4. write: <fd> <string> - Writes the specified string to the file associated
 *  with the specified file descriptor.
 *  5. seek: <fd> <offset> <whence> - Moves the file pointer to a specified
 * position in the file.
 *  6. ls: - Lists all files in the filesystem.
 *  7. wipe: - Wipes all files from the filesystem.
 *  8. create: <filename> - Creates a new file with the specified filename.
 *  9. rm: <filename> - Removes the file with the specified filename.
 *  10. format: <filename> - Formats the file with the specified filename.
 *  11. mv: <old_filename> <new_filename> - Moves a file from one location to
 *  another.
 *  12. cp: <source_filename> <destination_filename> - Copies a file from one
 *  location to another.
 *  13. exit: - exits
 *  14. test: - runs the unit tests
 *
 * @param command The command string to execute.
 */
int execute_command(char *command) {
    // Tokenize the command string to extract the command keyword
    char *token = strtok(command, " ");

    // Check if the command string is empty
    if (token == NULL) {
        printf("\nInvalid command\n");
        return 0;
    }

    // Compare the command keyword with known command strings and call the
    // corresponding handler function
    if (strcmp(token, "open") == 0) { // open: <filename> <mode>
        handle_open_command();
    } else if (strcmp(token, "close") == 0) { // close: <fd>
        handle_close_command();
    } else if (strcmp(token, "read") == 0) { // read: <fd> <size>
        handle_read_command();
    } else if (strcmp(token, "write") == 0) { // write: <fd> <string>
        handle_write_command();
    } else if (strcmp(token, "seek") == 0) { // seek: <fd> <offset> <whence>
        handle_seek_command();
    } else if (strcmp(token, "ls") == 0) { // ls
        handle_ls_command();
    } else if (strcmp(token, "wipe") == 0) { // wipe
        handle_wipe_command();
    } else if (strcmp(token, "create") == 0) { // create: <filename>
        handle_create_command();
    } else if (strcmp(token, "rm") == 0) { // rm: <filename>
        handle_rm_command();
    } else if (strcmp(token, "format") == 0) { // format: <filename>
        handle_format_command();
    } else if (strcmp(token, "mv") == 0) { // mv: <old_filename> <new_filename>
        handle_mv_command();
    } else if (strcmp(token, "cp") ==
               0) { // cp: <source_filename> <destination_filename>
        handle_cp_command();
    } else if (strcmp(token, "test") == 0) { // test
        run_tests();
    } else if (strcmp(token, "exit") == 0) { // exit
        return 1;
    } else {
        // If the command is not recognized, handle it as an unknown command
        handle_unknown_command();
    }
    return 0;
}

/**
 * @brief Handles the 'open' command to open a file with the specified name
 * and mode.
 *
 * This function parses the 'open' command and attempts to open a file with
 * the specified name and mode. It extracts the filename and mode from the
 * command string and calls the fs_open function to open the file. After
 * attempting to open the file, it prints appropriate messages based on the
 * result.
 *
 * @param token The tokenized command string containing the 'open' command
 * keyword.
 */
void handle_open_command() {
    // Extract the filename and mode from the command string
    char *token = strtok(NULL, " ");
    printf("token: %s\n", token);
    if (token == NULL) {
        printf("\nOpen needs a name\n");
        return;
    }
    char *name = token;

    // Extract the mode from the command string
    token = strtok(NULL, " ");
    printf("token: %s\n", token);
    if (token == NULL) {
        printf("\nOpen needs a mode\n");
        return;
    }
    int m = 0;

    // Check if the mode contains the 'c' character to indicate create mode
    if (strstr(token, "c") != NULL) {
        token[strlen(token) - 1] = '\0';
        m = MODE_CREATE;
    }
    // Check if the mode contains the 'r', 'w', or 'a' characters to
    // indicate read,
    if (strcmp(token, "r") == 0) {
        m |= MODE_READ;
    } else if (strcmp(token, "w") == 0) {
        m |= MODE_WRITE;
    } else if (strcmp(token, "rw") == 0) {
        m |= MODE_READ | MODE_WRITE;
    } else if (strcmp(token, "a") == 0) {
        m |= MODE_APPEND;
    } else if (strcmp(token, "aw") == 0) {
        m |= MODE_APPEND | MODE_WRITE;
    } else {
        printf("\nUnknown mode\n");
        return;
    }

    // Attempt to open the file with the specified name and mode
    int fd = fs_open(name, m);

    // Print appropriate messages based on the result of the fs_open
    // function
    if (fd == FILE_NOT_FOUND) {
        if (check_mode(m, MODE_CREATE)) {
            printf("\nFile not found and memory full\n");
        } else {
            printf("\nFile not found\n");
        }
    } else if (fd == FILE_ALREADY_OPEN) {
        printf("\nFile already opened\n");
    } else if (fd == OPENED_FILES_FULL) {
        printf("\nNo more file descriptors\n");
    } else if (fd == INCORRECT_MODE) {
        printf("\nCannot open in read and append mode\n");
    } else {
        printf("\nFile opened with fd %d\n", fd);
    }
}
/**
 * @brief Handles the 'close' command to close a file with the specified
 * file descriptor.
 *
 * This function parses the 'close' command and attempts to close the file
 * associated with the specified file descriptor. It extracts the file
 * descriptor from the command string and calls the fs_close function to
 * close the file.
 *
 * @param token The tokenized command string containing the 'close' command
 * keyword.
 */
void handle_close_command() {
    // Extract the file descriptor from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nClose needs a file descriptor\n");
        return;
    }
    int fd = atoi(token);
    fs_close(fd);
}

/**
 * @brief Handles the 'read' command to read data from a file with the
 * specified file descriptor.
 *
 * This function parses the 'read' command and attempts to read data from
 * the file associated with the specified file descriptor. It extracts the
 * file descriptor and the size of data to read from the command string,
 * reads the data from the file, and prints the read data or appropriate
 * error messages.
 *
 * @param token The tokenized command string containing the 'read' command
 * keyword.
 */
void handle_read_command() {
    // Extract the file descriptor and size from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRead needs a file descriptor\n");
        return;
    }
    int fd = atoi(token);
    // Extract the size of data to read from the command string
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRead needs a size\n");
        return;
    }
    int size = atoi(token);
    char buffer[size];
    int read = fs_read(fd, buffer, size);
    // Print appropriate messages based on the result of the fs_read
    // function
    if (read == FILE_NOT_OPEN) {
        printf("\nIncorrect file descriptor\n");
    } else if (read == INCORRECT_MODE) {
        printf("\nFile not open for reading\n");
    } else {
        printf("\nRead %d bytes: %.*s\n", read, read, buffer);
    }
}

/**
 * @brief Handles the 'write' command to write data to a file with the
 * specified file descriptor.
 *
 * This function parses the 'write' command and attempts to write data to
 * the file associated with the specified file descriptor. It extracts the
 * file descriptor and the string to write from the command string, writes
 * the data to the file, and prints the number of bytes written or
 * appropriate error messages.
 *
 * @param token The tokenized command string containing the 'write' command
 * keyword.
 */
void handle_write_command() {
    // Extract the file descriptor from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nWrite needs a file descriptor\n");
        return;
    }
    int fd = atoi(token);
    // Extract the string to write from the command string
    token = strtok(NULL, "\"");
    if (token == NULL) {
        printf("\nWrite needs a string\n");
        return;
    }
    int size = strlen(token);
    int written = fs_write(fd, token, size);
    // Print appropriate messages based on the result of the fs_write
    // function
    if (written == FILE_NOT_OPEN) {
        printf("\nIncorrect file descriptor\n");
    } else if (written == INCORRECT_MODE) {
        printf("\nFile not open for writing\n");
    } else if (written == OVERFLOW) {
        printf("\nData size exceeds maximum file size\n");
    } else {
        printf("\nWrote %d bytes\n", written);
    }
}

/**
 * @brief Handles the 'seek' command to move the file pointer to a specified
 * position in the file.
 *
 * This function parses the 'seek' command and attempts to move the file
 * pointer of the file associated with the specified file descriptor. It
 * extracts the file descriptor, offset, and whence parameter from the
 * command string, and calls the fs_seek function to perform the seek
 * operation. It prints appropriate messages based on the result of the seek
 * operation.
 *
 * @param token The tokenized command string containing the 'seek' command
 * keyword.
 */
void handle_seek_command() {
    // Extract the file descriptor from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs a file descriptor\n");
        return;
    }
    int fd = atoi(token);
    // Extract the offset parameter from the command string
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs an offset\n");
        return;
    }
    long offset = atoi(token);
    // Extract the whence parameter from the command string
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs a 'set', 'cur', or 'end' parameter\n");
        return;
    }
    int whence = 0;
    // Check the whence parameter to determine the seek position
    if (strcmp(token, "set") == 0) {
        whence = FS_SEEK_SET;
    } else if (strcmp(token, "cur") == 0) {
        whence = FS_SEEK_CUR;
    } else if (strcmp(token, "end") == 0) {
        whence = FS_SEEK_END;
    } else {
        printf("\nUnknown whence parameter\n");
        return;
    }
    int seek = fs_seek(fd, offset, whence);
    // Print appropriate messages based on the result of the fs_seek
    // function
    if (seek == FILE_NOT_OPEN) {
        printf("\nIncorrect file descriptor\n");
    } else {
        printf("\nSeeked to position %d\n", seek);
    }
}

/**
 * @brief Handles the 'ls' command to list all files in the filesystem.
 *
 * This function calls the fs_ls function to list all files in the
 * filesystem.
 */
void handle_ls_command() { printf("\nThe system has %d files\n", fs_ls()); }

/**
 * @brief Handles the 'wipe' command to wipe all files from the filesystem.
 *
 * This function calls the fs_wipe function to wipe all files from the
 * filesystem.
 */
void handle_wipe_command() { fs_wipe(); }

/**
 * @brief Handles the 'create' command to create a new file with the
 * specified name.
 *
 * This function parses the 'create' command and attempts to create a new
 * file with the specified name. It extracts the filename from the command
 * string and calls the fs_create function to create the file. It prints
 * appropriate messages based on the result of the create operation.
 *
 * @param token The tokenized command string containing the 'create' command
 * keyword.
 */
void handle_create_command() {
    // Extract the filename from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nCreate needs a name\n");
        return;
    }
    int create = fs_create(token);
    // Print appropriate messages based on the result of the fs_create
    // function
    if (create == FILE_ALREADY_EXISTS) {
        printf("\nFile already exists\n");
    } else if (create == FILE_TABLE_FULL) {
        printf("\nMemory is full\n");
    }
}

/**
 * @brief Handles the 'rm' command to remove a file with the specified name.
 *
 * This function parses the 'rm' command and attempts to remove the file
 * with the specified name. It extracts the filename from the command string
 * and calls the fs_rm function to remove the file. It prints appropriate
 * messages based on the result of the remove operation.
 *
 * @param token The tokenized command string containing the 'rm' command
 * keyword.
 */
void handle_rm_command() {
    // Extract the filename from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRemove needs a name\n");
        return;
    }
    // Call the fs_rm function to remove the file with the specified name
    if (fs_rm(token) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

/**
 * @brief Handles the 'format' command to format a file.
 *
 * This function parses the 'format' command and attempts to format the file
 * with the specified name. It extracts the filename from the command string
 * and calls the fs_format function to format the file. It prints
 * appropriate messages based on the result of the format operation.
 *
 * @param token The tokenized command string containing the 'format' command
 * keyword.
 */
void handle_format_command() {
    // Extract the filename from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nFormat needs a name\n");
        return;
    }
    // Call the fs_format function to format the file with the specified
    // name
    if (fs_format(token) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

/**
 * @brief Handles the 'mv' command to move a file from one location to
 * another.
 *
 * This function parses the 'mv' command and attempts to move a file from
 * one location to another. It extracts the source and destination filenames
 * from the command string and calls the fs_mv function to perform the move
 * operation. It prints appropriate messages based on the result of the move
 * operation.
 *
 * @param token The tokenized command string containing the 'mv' command
 * keyword.
 */
void handle_mv_command() {
    // Extract the source and destination filenames from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nMove needs 2 names\n");
        return;
    }
    // Extract the destination filename from the command string
    char *to = strtok(NULL, " ");
    if (to == NULL) {
        printf("\nMove needs 2 names\n");
        return;
    }
    // Call the fs_mv function to move the file from the source to the
    if (fs_mv(token, to) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

/**
 * @brief Handles the 'cp' command to copy a file from one location to
 * another.
 *
 * This function parses the 'cp' command and attempts to copy a file from
 * one location to another. It extracts the source and destination filenames
 * from the command string and calls the fs_cp function to perform the copy
 * operation. It prints appropriate messages based on the result of the copy
 * operation.
 *
 * @param token The tokenized command string containing the 'cp' command
 * keyword.
 */
void handle_cp_command() {
    // Extract the source and destination filenames from the command string
    char *token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nCopy needs 2 names\n");
        return;
    }
    // Extract the destination filename from the command string
    char *to = strtok(NULL, " ");
    if (to == NULL) {
        printf("\nCopy needs 2 names\n");
        return;
    }
    // Call the fs_cp function to copy the file from the source to the
    // destination
    if (fs_cp(token, to) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

/**
 * @brief Handles unknown commands by printing an appropriate message.
 *
 * This function is called when the command provided by the user is not
 * recognized. It simply prints a message indicating that the command is
 * unknown.
 */
void handle_unknown_command() { printf("\nUnknown command\n"); }
