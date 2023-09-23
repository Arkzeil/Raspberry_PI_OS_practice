#include <kernel/atags.h>

// VM does not emulate the bootloader which sets up the atags.
// So this would not work on VM
uint32_t get_mem_size(atag_t * tag) {
   while (tag->tag != NONE) {
       if (tag->tag == MEM) {
           return tag->mem.size;
       }
       tag = ((uint32_t *)tag) + tag->tag_size;
   }
   return 0;

}