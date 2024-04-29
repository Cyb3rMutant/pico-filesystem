#ifndef CLI_H
#define CLI_H

int execute_command(char *command);

void handle_open_command();
void handle_close_command();
void handle_read_command();
void handle_write_command();
void handle_seek_command();
void handle_ls_command();
void handle_wipe_command();
void handle_create_command();
void handle_rm_command();
void handle_format_command();
void handle_mv_command();
void handle_cp_command();
void handle_unknown_command();
#endif // CLI_H
