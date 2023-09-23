#include <stdint.h>
#include <kernel/atags.h>
#include <kernel/list.h>
#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096

// ':'is bit field initialization, the following struct would only occupy 1+1+30=32bits=4bytes
// still, it need aligned. So it would occupy 8 bytes if another 1 bits added
typedef struct {
	uint8_t allocated: 1;			// This page is allocated to something
	uint8_t kernel_page: 1;			// This page is a part of the kernel
	uint32_t reserved: 30;
} page_flags_t;

// create a linked list(see list.h) to keep track of which pages are free.
typedef struct page {
	uint32_t vaddr_mapped;	// The virtual address that maps to this page	
	page_flags_t flags;
	DEFINE_LINK(page);
} page_t;


void mem_init(atag_t * atags);

void * alloc_page(void);
void free_page(void * ptr);
#endif