// ref:https://jsandler18.github.io/extra/atags.html
// Atags is a list of information about certain aspects of the hardware. This list is created by the bootloader before our kernel is loaded. 
// The bootloader places it at address 0x100, passes that address to the kernel through register r2
// The Atags can tell us how large the memory is, where the bootloader put a ramdisk, what is the serial number of the board, and the command line passed to the kernel via cmdline.txt
// An Atag consists of a size (in 4 byte words), a tag identifier, and tag specific information. The list of Atags always starts with the CORE tag, with identifier 0x54410001, and ends with a NONE tag, with identifier 0.
// The tags are concatenated together, so the next tag in the list can be found by adding the number of bytes specified by the size to the current Atag’s pointer.

#include <stdint.h>
#ifndef ATAG_H
#define ATAG_H

typedef enum {
    NONE = 0x00000000,
    CORE = 0x54410001,
    MEM = 0x54410002,
    INITRD2 = 0x54420005,
    CMDLINE = 0x54410009,
} atag_tag_t;

typedef struct {
    uint32_t size;
    uint32_t start;
} mem_t;

// initrd (initial ramdisk) is a scheme for loading a temporary root file system into memory, to be used as part of the Linux startup process. 
typedef struct {
    uint32_t start;
    uint32_t size;
} initrd2_t;

typedef struct {
    char line[1];
} cmdline_t;

typedef struct atag {
    uint32_t tag_size;
    atag_tag_t tag;
    union {
        mem_t mem;
        initrd2_t initrd2;
        cmdline_t cmdline;
    };
} atag_t;

uint32_t get_mem_size(atag_t * atags);

#endif