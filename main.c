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
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test21();
    test22();
    test23();
    test24();
    test25();
    test26();
    test27();
    test28();
    test29();
    test30();
    test31();
    test32();
    test33();
    test34();
    test35();
    test36();
    test37();
    return 0;
}
