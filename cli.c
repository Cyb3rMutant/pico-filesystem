#include "cli.h"
#include "custom_fgets.h"
#include "filesystem.h"
#include "flash_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function: execute_command
// Parses and executes commands related to flash memory operations.
//
// Parameters:
// - command: A string containing the command and its arguments.
//
// The function supports the following commands:
// - FLASH_WRITE: Writes data to flash memory.
// - FLASH_READ: Reads data from flash memory.
// - FLASH_ERASE: Erases a sector of flash memory.
//
// Each command expects specific arguments following the command name.
void execute_command(char *command) {
    char *token = strtok(command, " ");
    if (token == NULL) {
        printf("\nInvalid command\n");
        return;
    }

    if (strcmp(token, "open") == 0) {
        handle_open_command(token);
    } else if (strcmp(token, "close") == 0) {
        handle_close_command(token);
    } else if (strcmp(token, "read") == 0) {
        handle_read_command(token);
    } else if (strcmp(token, "write") == 0) {
        handle_write_command(token);
    } else if (strcmp(token, "seek") == 0) {
        handle_seek_command(token);
    } else if (strcmp(token, "ls") == 0) {
        handle_ls_command();
    } else if (strcmp(token, "wipe") == 0) {
        handle_wipe_command();
    } else if (strcmp(token, "create") == 0) {
        handle_create_command(token);
    } else if (strcmp(token, "rm") == 0) {
        handle_rm_command(token);
    } else if (strcmp(token, "format") == 0) {
        handle_format_command(token);
    } else if (strcmp(token, "mv") == 0) {
        handle_mv_command(token);
    } else if (strcmp(token, "cp") == 0) {
        handle_cp_command(token);
    } else {
        handle_unknown_command();
    }
}

void handle_open_command(char *token) {
    token = strtok(NULL, " ");
    printf("token: %s\n", token);
    if (token == NULL) {
        printf("\nOpen needs a name\n");
        return;
    }
    char *name = token;
    token = strtok(NULL, " ");
    printf("token: %s\n", token);
    if (token == NULL) {
        printf("\nOpen needs a mode\n");
        return;
    }
    int m = 0;
    if (strstr(token, "c") != NULL) {
        token[strlen(token) - 1] = '\0';
        m = MODE_CREATE;
    }
    if (strcmp(token, "r") == 0) {
        m |= MODE_READ;
    } else if (strcmp(token, "w") == 0) {
        m |= MODE_WRITE;
    } else if (strcmp(token, "rw") == 0) {
        m |= MODE_READ | MODE_WRITE;
    } else {
        printf("\nUnknown mode\n");
        return;
    }

    int fd = fs_open(name, m);
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
        printf("\nCant open in read and append\n");
    } else {
        printf("\nFile opened with fd %d\n", fd);
    }
}

void handle_close_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nClose needs a fd\n");
        return;
    }
    int fd = atoi(token);
    fs_close(fd);
}

void handle_read_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRead needs a fd\n");
        return;
    }
    int fd = atoi(token);
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRead needs a size\n");
        return;
    }
    int size = atoi(token);
    char buffer[size];
    int read = fs_read(fd, buffer, size);
    if (read == FILE_NOT_OPEN) {
        printf("\nIncorrect fd\n");
    } else if (read == INCORRECT_MODE) {
        printf("\nFile not open for reading\n");
    } else {
        printf("\nRead %d bytes: %.*s\n", read, read, buffer);
    }
}

void handle_write_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nWrite needs a fd\n");
        return;
    }
    int fd = atoi(token);
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nWrite needs a string\n");
    }
    int size = strlen(token);
    int written = fs_write(fd, token, size);
    if (written == FILE_NOT_OPEN) {
        printf("\nIncorrect fd\n");
    } else if (written == INCORRECT_MODE) {
        printf("\nFile not open for writing\n");
    } else if (written == OVERFLOW) {
        printf("\nData bigger than max file size\n");
    } else {
        printf("\nWrote %d bytes\n", written);
    }
}

void handle_seek_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs a fd\n");
        return;
    }
    int fd = atoi(token);
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs a offset\n");
        return;
    }
    long offset = atoi(token);
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nSeek needs a whence\n");
        return;
    }
    int whence = 0;
    if (strcmp(token, "set") == 0) {
        whence = FS_SEEK_SET;
    } else if (strcmp(token, "cur") == 0) {
        whence = FS_SEEK_CUR;
    } else if (strcmp(token, "end") == 0) {
        whence = FS_SEEK_END;
    } else {
        printf("\nUnknown whence\n");
        return;
    }
    int seek = fs_seek(fd, offset, whence);
    if (seek == FILE_NOT_OPEN) {
        printf("\nIncorrect fd\n");
    } else {
        printf("\nSeeked to %d\n", seek);
    }
}

void handle_ls_command() { fs_ls(); }

void handle_wipe_command() { fs_wipe(); }

void handle_create_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nCreate needs a name\n");
        return;
    }
    int create = fs_create(token);
    if (create == FILE_ALREADY_EXISTS) {
        printf("\nFile already exists\n");
    } else if (create == FILE_TABLE_FULL) {
        printf("\nMemory is full\n");
    }
}

void handle_rm_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nRemove needs a name\n");
        return;
    }

    if (fs_rm(token) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

void handle_format_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nFormat needs a name\n");
        return;
    }

    if (fs_format(token) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

void handle_mv_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nMove needs 2 names\n");
        return;
    }
    char *to = strtok(NULL, " ");
    if (to == NULL) {
        printf("\nMove needs 2 names\n");
        return;
    }

    if (fs_mv(token, to) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

void handle_cp_command(char *token) {
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("\nCopy needs 2 names\n");
        return;
    }
    char *to = strtok(NULL, " ");
    if (to == NULL) {
        printf("\nCopy needs 2 names\n");
        return;
    }

    if (fs_cp(token, to) == FILE_NOT_FOUND) {
        printf("\nFile not found\n");
    }
}

void handle_unknown_command() { printf("\nUnknown command\n"); }