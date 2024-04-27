#ifndef CLI_H
#define CLI_H

void execute_command(char *command);
void handle_open_command(char *token);
void handle_close_command(char *token);
void handle_read_command(char *token);
void handle_write_command(char *token);
void handle_seek_command(char *token);
void handle_ls_command();
void handle_wipe_command();
void handle_create_command(char *token);
void handle_rm_command(char *token);
void handle_format_command(char *token);
void handle_mv_command(char *token);
void handle_cp_command(char *token);
void handle_unknown_command();
#endif // CLI_H
