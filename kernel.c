//set up the hardware for basic I/O(UART?)

#include <stddef.h>
#include <stdint.h>

static inline void mmio_write(uint32_t reg, uint32_t data){ //MMIO(Memory Mapped IO) : all interactions with hardware on the Raspberry Pi occur using MMIO.
    //vollatile: get the variable from memory directly, instead from register(which may resulted from compiler optimization)
    //see: https://ithelp.ithome.com.tw/articles/10308388
    return *(volatile uint32_t*)reg = data; 
}

static inline void mmio_read(uint32_t reg){
    return *(volatile uint32_t*)reg;
}

// giving the hardware some time to respond to any writes we may have made.(This is an imprecise way)
static inline void delay(int32_t count){
    // https://hackmd.io/@happy-kernel-learning/S1jo0eB2L
    // "%=" Outputs a number that is unique to each instance of the asm statement in the entire compilation. This option is useful when creating local labels and referring to them multiple times in a single template that generates multiple assembler instructions
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n" // count subtract 1 then jump to delay if it's not 0
                : "=r"(count) // output operand, store value to count after subtration
                : [count]"0"(count) // input operand, [a symbolic name]constraint(c expression)
                : "cc");    // clobbered register, used to notiyfy compiler that the register may changed. cc means assmebly codes mofified the flag register
}

// peripheral offset of the GPIO and the UART hardware systems, as well as some of their registers.
enum
{
    // The GPIO registers base address.
    GPIO_BASE = 0x3F200000, // for raspi2 & 3, 0x20200000 for raspi1

    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = 0x3F201000, // for raspi2 & 3, 0x20201000 for raspi1

    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

// set up the UART hardware
void uart_init()
{
    // disables all aspects of the UART hardware. UART0_CR is the UART’s Control Register.
    mmio_write(UART0_CR, 0x00000000);

    // GPIO Pull/Down Register, need to work with GPPUDCLK, pins should be disabled. 
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // marks which pins should be disabled
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // makes the whole thing take effect
    mmio_write(GPPUDCLK0, 0x00000000);

    // sets all flags in the Interrupt Clear Register. This has the effect of clearing all pending interrupts from the UART hardware.
    mmio_write(UART0_ICR, 0x7FF);

    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c)
{
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");

    while (1) {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}



