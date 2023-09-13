//main ref: https://jsandler18.github.io/explanations/kernel_c.html
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

// set up the UART hardware, this practice isn't actually using GPIO pins
void uart_init()
{
    // disables all aspects of the UART hardware. UART0_CR is the UART’s Control Register.
    mmio_write(UART0_CR, 0x00000000);

    // GPIO Pull/Down Register, need to work with GPPUDCLK, pins should be disabled. 
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // marks which pins should be disabled(for those also having value 0 in GPPUD)
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // makes the whole thing take effect
    mmio_write(GPPUDCLK0, 0x00000000);

    // sets all flags in the Interrupt Clear Register. This has the effect of clearing all pending interrupts from the UART hardware.
    mmio_write(UART0_ICR, 0x7FF);

    // IBDD : Integer Baud rate divisor 
    // sets the baud rate(bits/sec) of the connection, ref:https://juejin.cn/post/6977611730784354334
    // BAUD = CLOCK_SPEED/(16*USART_DIV) -> USART_DIV = UART_CLOCK_SPEED/(16 * DESIRED_BAUD), which DESIRED_BAUD=115200 here
    // tutorial didn't give the clock speed, 115200*16*1.67 = 3078144 is the clock speed?
    mmio_write(UART0_IBRD, 1);
    // FBRD : Fractional Baud rate divisor 
    // FBRD = INTEGER( (fraction_part * 64) + 0.5 ) , (.67 * 64) + .5 = 40
    mmio_write(UART0_FBRD, 40);

    // Line control register. Setting bit 4 means that the UART hardware will hold data in an 8 item deep FIFO, instead of a 1 item deep register. 
    // Setting 5 and 6 to 1 means that data sent or received will have 8-bit long words.
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // disables all interrupts from the UART by writing a one to the relevent bits of the Interrupt Mask Set Clear register.
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    // writes bits 0, 8, and 9 to the control register. Bit 0 enables the UART hardware, bit 8 enables the ability to receive data, and bit 9 enables the ability to transmit data.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

// doc p.165 : https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
// FR:flag reggister(tells us whether the read FIFO has any data for us to read, and whether the write FIFO can accept any data.) 
// DR:data register(where data is both read from and written to)
// following 2 functions enable reading and writing characters to and from the UART.
void uart_putc(unsigned char c)
{
    // get value to know if  Receive is busy
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
    // wait until TX fifo is not empty
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}


// this is where control is transfered to from boot.S
// print out any character you type. This is where we will add calls to many other initialization functions.
// In ARM, the convention is that the first three parameters of a function are passed through registers r0, r1 and r2.
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

// When the bootloader loads our kernel, it also places some information about the hardware and the command line used to run the kernel in memory.
// This information is called atags, and a pointer to the atags is placed in r2 just before boot.S runs. 
// So for our kernel_main, r0 and r1 are parameters to the function simply by convention, but we don’t care about those. r2 contains the atags pointer, so the third argument to kernel_main is the atags pointer.

