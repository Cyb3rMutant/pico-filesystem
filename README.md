# Intro

A file system (FS) controls file organization and access. A local file system is an operating system feature that enables apps running on the same machine to access a shared mass storage. This project aims to construct a simple yet reliable file system for the Raspberry Pi Pico flash memory.

# How to use

First, clone the repository locally with the following command:

```bash
$ git clone https://gitlab.uwe.ac.uk/y2-abuhummos/cap_fs
$ cd cap_fs
```

next a build, run and view output scripts are provided, to build:

```bash
$ ./build.sh
```

to run, first make sure to unplug the pi, and plug it again with the reset button pressed then:

```bash
$ ./run.sh
```

otherwise you can drag and drop the file into the pi.

and finally to view output:

```bash
$ ./out.sh
```

# Architecture

The flash memory on the Pi is partitioned into sectors, sequentially numbered from 1 onwards. This filesystem utilizes sector 0 as a file allocation table (FAT), responsible for managing metadata such as file names, sizes, and usage status.

Additionally, an in-memory array tracks open files and their associated data, including position and mode, which will be discussed later on.

## File Allocation Table (FAT) Block

The zeroth sector, or block, within the flash memory serves as an array table of `FileEntry` structures. Each structure corresponds to a sector at its respective index, storing essential metadata. At index 0, occupied by this array, the `filename` field is repurposed to store a specific magic string: `"magic string for initializing\0"`. During filesystem initialization, if this string is absent or incorrect, this means the file system is corrupt or has not been set up before so the filesystem initializes the table and writes it back to memory. The image below illustrates how it looks like:

![FAT structure](./img/FAT-structure.jpg)

## Open Files Table

This table maintains a record of opened files, where each entry's index represents its file descriptor. Users interact with the system using these descriptors. An entry is deemed open when its corresponding file entry pointer is set to `NULL`.

# Implementation and Design Decisions

## Error Handling

All return types within this system are integers. A positive integer indicates success, with zero just denoting success and any value above zero representing additional information such as the number of bytes or a file descriptor. Negative values signify errors, with each error type mapped to a specific negative number within an enum. This approach enhances readability, as illustrated in the example below from `fs_create`:

```c
    // Check if file already exists
    if (get_file(path) != FILE_NOT_FOUND) {
        return FILE_ALREADY_EXISTS;
    }
```

## File Modes

File modes are defined using bitwise shifting of binary digits instead of enums. This allows for combining multiple modes using bitwise OR. For example, to open a file for both reading and writing:

```c
fs_open("file_name", MODE_READ | MODE_WRITE);
```

Bitwise OR retains all set bits:

```
0001 // read mode
    +
0010 // write mode
----
0011 // read + write mode
```

To check if a mode is applied, bitwise AND is used:

```
0101 // read + append
    &
0010 // append mode
----
0000 // append mode is not applied
```

## Create

Files in the FAT table are considered to exist if they have a non-null filename. Therefore, the create function loops over the file table to find an entry with a null filename.

## Remove

Removing a file involves setting its filename and other associated fields to null or zero.

## Format

Formatting clears the contents of the specified file stored on the flash and sets its size to zero.

## Wipe

Wiping performs a hard reset of the flash memory, clearing all sectors and resetting all file metadata in the FAT table to zero.

## Copy

The copy operation creates a duplicate of the source file in the destination, overwriting its content if it already exists or creating a new file if it doesn't. This results in two files at the end.

## Move

Similar to copy, move copies the source to the destination if it exists. However, unlike copy, it deletes the source file afterward, resulting in only one file. If the destination file doesn't exist, move simply renames the source file.

## List

The list operation is straightforward: if the file exists, it prints its metadata; otherwise, it moves on to the next entry in the FAT table.

## Open

Open retrieves the file from the FAT table then assigns it a file descriptor based on the first free index in the `opened_files` array and returns that fd. All further interactions with that opened file will be through that fd.

## Close

Similar to Remove, closing a file is done by null-ing the entry in the opened files array at that fd index

## Read

This is where the `position` field starts to be used. read acts as a buffer, so everytime the user reads however many bytes, the position will move by that much forward, and the next read/write will occure from that point.

so if we have the string `"Hello world!"` stored in a file and read 6 bytes twice like this:

```c
fs_read(1, buf, 6);
fs_read(1, buf, 6);
```

we will get this:

```
"hello " // first read
"world!" // second read
```

The way this works is that theres a pre allocated temp buffer used to read everything written into the file, then filtering that out using the `position` field into the buffer provided through the function call. The reason this is done is that the methods provided for reading and writing to memory dont offer an offset within the sector itself

## Write

Write has 2 modes, depending on which the file has been opened with:

### normal writing

this one acts in the same way read does with the position, so it reads the file, copies the provided buffer into the temp buffer then writes it back to the flash, moving the position forward.

For example if we had a file that had `"Hello world!"` written to it and the position was 3 and we wrote hello again to it the content will become `"Helhellorld!"` and position will become 8

### append

append stores the current position, then sets the position to the end of the file, and writes the content there, then retains the old position

So if we had the same file with `"Hello world!"` at position 3, and appended `"hello"` to it, it will become `"Hello world!hello` and position remains 3

## Seek

Seek is used to manipulate the position, it has 3 modes:

- **SET**: it sets the position to whatever offset given from the start of the file moving forward, example:

  ```c
  // cur pos is 5, and size is 10
  fs_seek(1, 3, SEEK_SET);
  // cur pos is 3
  ```

- **CUR**: it sets the position to whatever offset given from the current position moving forward

  ```c
  // cur pos is 5, and size is 10
  fs_seek(1, 3, SEEK_CUR);
  // cur pos is 8
  ```

- **END**: it sets the position to whatever offset given from the end of the file backwards
  ```c
  // cur pos is 5, and size is 10
  fs_seek(1, 3, SEEK_END);
  // cur pos is 7
  ```
