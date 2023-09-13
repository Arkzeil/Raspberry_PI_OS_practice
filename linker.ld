/* '.' :location counter, means current address. 0 at the start of SECTIONS */
/* main ref:https://wiki.osdev.org/Raspberry_Pi_Bare_Bones */
/* see https://blog.louie.lu/2016/11/06/10%E5%88%86%E9%90%98%E8%AE%80%E6%87%82-linker-scripts/ for linker script explaination */

/* set entry point, where the program start to execute first instruction */
ENTRY(_start)
 
SECTIONS
{
    /* Starts at LOADER_ADDR. */
    /* set current location as 0x8000 ,current address is automatically incremented when the linker adds data. */
    . = 0x8000;

    /* creates a symbols '__start' ,'__text_start', sets '__start' to the current address.  */
    __start = .;
    __text_start = .;

    /* define output sections '.text' */
    /* stuffs in the curly bracket are the content to put into .text output section */
    .text :
    {
        /* 'KEEP' prevents from link-time garbage collection */
        /* In our case ".text.boot" is to be placed first followed by the more general ".text". ".text.boot" is only used in boot.S and ensures that it ends up at the beginning of the kernel image.  */
        KEEP(*(.text.boot))
        /* all .text input section in input files, '*' is wildcard to match all matched files */
        *(.text)
    }
    /* align to page size ,which is 4096 for the RPi */
    . = ALIGN(4096); 

    /* creates a __text_end symbol so we know where the section ends. */
    __text_end = .;

    /* '__text_start' and '__text_end' can be used in the kernel source and the linker will then place the correct addresses into the binary. */
 
    /* read only data; it is where global constants are placed. */
    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;

    /* __bss_start and __bss_end are used in boot.S. */

    __end = .;
}