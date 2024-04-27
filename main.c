// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "flash_ops.h"
// #include "filesystem.h"
// const uint LED_PIN = 25;

// int main() {
//     stdio_init_all();
//     gpio_init(LED_PIN);
//     gpio_set_dir(LED_PIN, GPIO_OUT);
//     while (1) {
//         gpio_put(LED_PIN, 0);
//         sleep_ms(250);
//         gpio_put(LED_PIN, 1);
//         printf("Hello World\n");
//         sleep_ms(1000);
//     }
// }
// #include "cli.h"
// #include "custom_fgets.h"
#include "cli.h"
#include "custom_fgets.h"
#include "filesystem.h"
#include "flash_ops.h"
#include "pico/stdlib.h"
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
    while (1) {
        printf("\nEnter command: ");
        custom_fgets(command, sizeof(command), stdin);
        execute_command(command);
    }
    // fs_ls();
    // FS_FILE *fd = fs_open("3.txt", MODE_READ | MODE_WRITE);
    // char w_buf[4096] = "Hello World\0";
    // fs_write(fd, w_buf, 12);
    // fs_ls();
    //
    // // printf("%d\n", fs_write(fd_4, buffer, 20));
    // // printf("%d\n", fs_write(fd_2, "how are you", 11));
    // // printf("%d\n", fs_write(fd, "i am ggoodd", 11));
    // // fs_read(fd_1, buffer, 11);
    // // printf("Buffer: %s\n", buffer);
    // char r_buf[4096];
    // fs_read(fd, r_buf, 12);
    // printf("Buffer: %s\n", r_buf);
    // // fs_read(fd, buffer, 11);
    // // printf("Buffer: %s\n", buffer);
    // fs_ls();
    //
    return 0;
}
