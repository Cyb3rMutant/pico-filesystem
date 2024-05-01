#include "tests.h"
#include "filesystem.h"

void run_tests() {
    for (int i = 0; i < 10; i++) {
        fs_close(i);
    }

    fs_wipe();
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
    for (int i = 0; i < 10; i++) {
        fs_close(i);
    }
    fs_wipe();
}

void test1() {
    // Test 1: Create a file
    printf("Test 1: Create a file\n");
    if (fs_create("file1") == 1) {
        printf("Test 1: Passed\n");
    } else {
        printf("Test 1: Failed\n");
    }
}

void test2() {
    // Test 2: Create a file with the same name
    printf("Test 2: Create a file with the same name\n");
    if (fs_create("file1") == FILE_ALREADY_EXISTS) {
        printf("Test 2: Passed\n");
    } else {
        printf("Test 2: Failed\n");
    }
}

void test3() {
    fs_wipe();
    // Test 3: Fill the filesystem with files and try to create a new one
    printf(
        "Test 3: Fill the filesystem with files and try to create a new one\n");
    for (int i = 0; i < 25; i++) {
        char filename[6];
        sprintf(filename, "file%d", i);
        fs_create(filename);
    }
    if (fs_create("file26") == FILE_TABLE_FULL) {
        printf("Test 3: Passed\n");
    } else {
        printf("Test 3: Failed\n");
    }
    fs_wipe();
}

void test4() {
    // Test 4: Create a file after wiping the filesystem
    printf("Test 4: Create a file after wiping the filesystem\n");
    fs_create("file1");
    fs_wipe();
    if (fs_create("file1") == 1) {
        printf("Test 4: Passed\n");
    } else {
        printf("Test 4: Failed\n");
    }
}
void test5() {
    // Test 5: Move a file to a new location
    printf("Test 5: Move a file to a new location\n");
    fs_create("file1");
    fs_mv("file1", "file2");
    if (fs_ls() == 1) {
        printf("Test 5: Passed\n");
    } else {
        printf("Test 5: Failed\n");
    }
}
void test6() {
    // Test 6: Move a file to a location that already exists
    printf("Test 6: Move a file to a location that already exists\n");
    fs_create("file1");
    fs_create("file2");
    fs_mv("file1", "file2");
    if (fs_ls() == 1) {
        printf("Test 6: Passed\n");
    } else {
        printf("Test 6: Failed\n");
    }
}
void test7() {
    // Test 7: Move a file to the same location
    printf("Test 7: Move a file to the same location\n");
    fs_wipe();
    fs_create("file1");
    fs_mv("file1", "file1");
    if (fs_ls() == 1) {
        printf("Test 7: Passed\n");
    } else {
        printf("Test 7: Failed\n");
    }
}
void test8() {
    // Test 8: Copy a file to a new location
    printf("Test 8: Copy a file to a new location\n");
    fs_cp("file1", "file2");
    if (fs_ls() == 2) {
        printf("Test 8: Passed\n");
    } else {
        printf("Test 8: Failed\n");
    }
}
void test9() {
    // Test 9: Copy a file to a location that already exists
    printf("Test 9: Copy a file to a location that already exists\n");
    fs_cp("file1", "file2");
    if (fs_ls() == 2) {
        printf("Test 9: Passed\n");
    } else {
        printf("Test 9: Failed\n");
    }
}
void test10() {
    // Test 10: Copy a file to the same location
    printf("Test 10: Copy a file to the same location\n");
    fs_wipe();
    fs_create("file1");
    fs_cp("file1", "file1");
    if (fs_ls() == 1) {
        printf("Test 10: Passed\n");
    } else {
        printf("Test 10: Failed\n");
    }
}
void test11() {
    // Test 11: Remove a file
    printf("Test 11: Remove a file\n");
    fs_rm("file1");
    if (fs_ls() == 0) {
        printf("Test 11: Passed\n");
    } else {
        printf("Test 11: Failed\n");
    }
}
void test12() {
    // Test 12: Remove a file that doesn't exist
    printf("Test 12: Remove a file that doesn't exist\n");
    if (fs_rm("file1") == FILE_NOT_FOUND) {
        printf("Test 12: Passed\n");
    } else {
        printf("Test 12: Failed\n");
    }
}
void test13() {
    // Test 13: Remove a file after wiping the filesystem
    printf("Test 13: Remove a file after wiping the filesystem\n");
    fs_create("file1");
    fs_wipe();
    if (fs_rm("file1") == FILE_NOT_FOUND) {
        printf("Test 13: Passed\n");
    } else {
        printf("Test 13: Failed\n");
    }
}
void test14() {
    // Test 14: Create a file after removing one with the same name
    printf("Test 14: Create a file after removing one\n");
    fs_create("file1");
    fs_rm("file1");
    if (fs_create("file1") == 1) {
        printf("Test 14: Passed\n");
    } else {
        printf("Test 14: Failed\n");
    }
}
void test15() {
    // Test 15: Open a file
    printf("Test 15: Open a file\n");

    if (fs_open("file1", MODE_READ) == 0) {
        printf("Test 15: Passed\n");
    } else {
        printf("Test 15: Failed\n");
    }
    fs_close(0);
}
void test16() {
    // Test 16: Open a file that doesn't exist
    printf("Test 16: Open a file that doesn't exist\n");
    if (fs_open("file2", MODE_READ) == FILE_NOT_FOUND) {
        printf("Test 16: Passed\n");
    } else {
        printf("Test 16: Failed\n");
    }
}
void test17() {
    // Test 17: Open a file with write and append mode
    printf("Test 17: Open a file with write and append mode\n");
    if (fs_open("file1", MODE_WRITE | MODE_APPEND) == INCORRECT_MODE) {
        printf("Test 17: Passed\n");
    } else {
        printf("Test 17: Failed\n");
    }
}
void test18() {
    // Test 18: Open a file that is already open
    printf("Test 18: Open a file that is already open\n");
    fs_create("file1");
    fs_open("file1", MODE_READ);
    if (fs_open("file1", MODE_READ) == FILE_ALREADY_OPEN) {
        printf("Test 18: Passed\n");
    } else {
        printf("Test 18: Failed\n");
    }
    fs_close(0);
}
void test19() {
    // Test 19: Open a file that doesn't exist with create mode
    printf("Test 19: Open a file that doesn't exist with create mode\n");
    fs_wipe();
    if (fs_open("file1", MODE_CREATE) == 0) {
        printf("Test 19: Passed\n");
    } else {
        printf("Test 19: Failed\n");
    }
    fs_close(0);
}
void test20() {
    // Test 20: Open a file that is already open
    printf("Test 20: Open a file that is already open\n");
    fs_open("file1", MODE_READ);
    if (fs_open("file1", MODE_READ) == FILE_ALREADY_OPEN) {
        printf("Test 20: Passed\n");
    } else {
        printf("Test 20: Failed\n");
    }
    fs_close(0);
}
void test21() {
    // Test 21: fill the filesystem with files and try to open a new one
    printf(
        "Test 21: Fill the filesystem with files and try to open a new one\n");
    for (int i = 0; i < 10; i++) {
        char filename[6];
        sprintf(filename, "file%d", i);
        fs_open(filename, MODE_CREATE);
    }
    if (fs_open("file26", MODE_CREATE) == OPENED_FILES_FULL) {
        printf("Test 21: Passed\n");
    } else {
        printf("Test 21: Failed\n");
    }
    for (int i = 0; i < 10; i++) {
        fs_close(i);
    }
    fs_wipe();
}
void test22() {
    // Test 22: Open a file after closing it
    printf("Test 22: Open a file after closing it\n");
    fs_open("file1", MODE_CREATE | MODE_READ);
    fs_close(0);
    if (fs_open("file1", MODE_READ) == 0) {
        printf("Test 22: Passed\n");
    } else {
        printf("Test 22: Failed\n");
    }
    fs_close(0);
}
void test23() {
    // Test 23: Write to a file
    printf("Test 23: Write to a file\n");
    fs_open("file1", MODE_WRITE);
    char buffer[10] = "test";
    if (fs_write(0, buffer, 4) == 4) {
        printf("Test 23: Passed\n");
    } else {
        printf("Test 23: Failed\n");
    }
    fs_close(0);
}
void test24() {
    // Test 24: read from a file
    printf("Test 24: Read from a file\n");
    fs_open("file1", MODE_READ);
    char buffer[10];
    if (fs_read(0, buffer, 4) == 4) {
        printf("Test 24: Passed\n");
    } else {
        printf("Test 24: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test25() {
    // Test 25: Read from a file that has just been written to
    printf("Test 25: Read from a file that has just been written to\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    char read_buffer[10];
    fs_seek(0, 0, FS_SEEK_SET);
    if (fs_read(0, read_buffer, 4) == 4) {
        printf("Test 25: Passed\n");
    } else {
        printf("Test 25: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test26() {
    // Test 26: Read from a file that has been written to multiple times
    printf(
        "Test 26: Read from a file that has been written to multiple times\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_write(0, buffer, 4);
    char read_buffer[10];
    fs_seek(0, 0, FS_SEEK_SET);
    if (fs_read(0, read_buffer, 8) == 8) {
        printf("Test 26: Passed\n");
    } else {
        printf("Test 26: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test27() {
    // Test 27: Read multiple times from a file
    printf("Test 27: Read multiple times from a file\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "testtest";
    fs_write(0, buffer, 8);
    char read_buffer[10];
    fs_seek(0, 0, FS_SEEK_SET);
    if (fs_read(0, read_buffer, 4) == 4) {
        if (fs_read(0, read_buffer, 4) == 4) {
            printf("Test 27: Passed\n");
        } else {
            printf("Test 27: Failed\n");
        }
    } else {
        printf("Test 27: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test28() {
    // Test 28: Read from a file after writing without seeking
    printf("Test 28: Read from a file after writing without seeking\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 0) {
        printf("Test 28: Passed\n");
    } else {
        printf("Test 28: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test29() {
    // Test 29: Read from a file after writing with seeking to the middle
    printf(
        "Test 29: Read from a file after writing with seeking to the middle\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    char read_buffer[10];
    fs_seek(0, 2, FS_SEEK_SET);
    if (fs_read(0, read_buffer, 4) == 2) {
        printf("Test 29: Passed\n");
    } else {
        printf("Test 29: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test30() {
    // Test 30: Append to a file
    printf("Test 30: Append to a file\n");
    fs_open("file1", MODE_CREATE | MODE_READ | MODE_APPEND);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 4) {
        printf("Test 30: Passed\n");
    } else {
        printf("Test 30: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test31() {
    // Test 31: Move moves file content too
    printf("Test 31: Move moves file content too\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_close(0);
    fs_mv("file1", "file2");
    fs_open("file2", MODE_READ);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 4) {
        printf("Test 31: Passed\n");
    } else {
        printf("Test 31: Failed\n");
    }
    fs_close(0);
    fs_rm("file2");
}
void test32() {
    // Test 32: Copy copies file content too
    printf("Test 32: Copy copies file content too\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_cp("file1", "file2");
    fs_open("file2", MODE_READ);
    char read_buffer[10];
    int x = fs_read(1, read_buffer, 4);
    printf("%d", x);
    if (x == 4) {
        printf("Test 32: Passed\n");
    } else {
        printf("Test 32: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
    fs_close(1);
    fs_rm("file2");
}
void test33() {
    // Test 33: Writing from the middle of a file
    printf("Test 33: Writing from the middle of a file\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_seek(0, 2, FS_SEEK_SET);
    fs_write(0, buffer, 4);
    char read_buffer[10];
    fs_seek(0, 0, FS_SEEK_SET);
    if (fs_read(0, read_buffer, 6) == 6) {
        printf("Test 33: Passed\n");
    } else {
        printf("Test 33: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test34() {
    // Test 34: Writing in the end of a file
    printf("Test 34: Writing in the end of a file\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_seek(0, 1, FS_SEEK_SET);
    fs_write(0, buffer, 2);
    fs_seek(0, 0, FS_SEEK_SET);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 4) {
        printf("Test 34: Passed\n");
    } else {
        printf("Test 34: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test35() {
    // Test 35: writing after formating a file
    printf("Test 35: writing after formating a file\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_format("file1");
    fs_seek(0, 0, FS_SEEK_SET);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 0) {
        fs_write(0, buffer, 4);
        printf("Test 35: Passed\n");
    } else {
        printf("Test 35: Failed\n");
    }
    fs_close(0);
    fs_rm("file1");
}
void test36() {
    // Test 36: Copying overwrites file content
    printf("Test 36: Copying overwrites file content\n");
    fs_open("file1", MODE_CREATE | MODE_WRITE | MODE_READ);
    char buffer[10] = "test";
    fs_write(0, buffer, 4);
    fs_create("file2");
    fs_cp("file2", "file1");
    fs_seek(0, 0, FS_SEEK_SET);
    char read_buffer[10];
    if (fs_read(0, read_buffer, 4) == 0) {
        fs_write(0, buffer, 4);
        printf("Test 36: Passed\n");
    } else {
        printf("Test 36: Failed\n");
    }
}
void test37() {
    // Test 37: Moving deletes the source file
    printf("Test 37: Moving deletes the source file\n");
    fs_create("file1");
    fs_mv("file1", "file2");
    if (fs_open("file1", MODE_READ) == FILE_NOT_FOUND) {
        printf("Test 37: Passed\n");
    } else {
        printf("Test 37: Failed\n");
    }
}
