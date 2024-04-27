#include "cli.h"
#include "custom_fgets.h"
#include "filesystem.h"
#include "flash_ops.h"
#include "pico/stdlib.h"
#include "tests.h"
#include <stdio.h>

int main() {
    stdio_init_all();
    char command[256];

    // Wait for USB connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    init_filesystem();

    // Command loop
    // while (1) {
    //     printf("\nEnter command: ");
    //     custom_fgets(command, sizeof(command), stdin);
    //     execute_command(command);
    // }
    run_tests();
    return 0;
}
