@ref : https://jsandler18.github.io/explanations/boot_S.html

.section ".text.book"   @where this code belongs in the compiled binary.

.global _start          @The name that's visible from outside of the assembly file

_start:
    @Move to ARM register from coprocessor(CP15, for storage manipulation): MRC{cond} coproc, opcode1, Rd, CRn, CRm{, opcode2}  @for cp15 register, op1 should be 0 
    @this line should be putting the CPI ID into r1 register
    mrc p15, #0, r1, c0, c0, #5 
    @keep the one CPU marked 3, shut down others
    and r1, r1, #3
    @if is not the #3 CPU, sent to halt
    cmp r1, #0
    bne halt

    @C stack should start at address 0x8000 and grow downwards, since hardware loads our kernel to address 0x8000 and up, stack can safely run from 0x8000 and down
    mov sp, #0x8000
    @BSS is where C global variables that are not initialized at compile time are stored. 
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    @ C runtime requires that uninitialized global variables are zero, so we must zero out this entire section ourselves. 
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    @xf(b) : branches to the first found label "x" searching "forward" for "f" or "backward" for "b".(GNU specific) see:https://stackoverflow.com/questions/27353096/1b-and-1f-in-gnu-assembly
    b       2f

1:
    @store much increase after(increase 4 after transmitting addr. , from left to right). The ! means store that address back in r4, as opposed to throwing it out
    @ store the values in the consecutive registers r5,r6,r7,r8 (so 16 bytes) into r4. So overall, the instruction stores 16 bytes of zeros into the address in r4
    @this loops until r4 is greater than or equal to r9, and the whole BSS section is zeroed out.
    stmia r4!, {r5-r8}

2:
    cmp r4, r9
    @equal to BCC? Branch On Lower(unsigned)
    blo 1b
    @ loads the address of the C function called kernel_main into a register and jumps to that location
    ldr r3, =kernel_main
    @Branch with Link, and optionally exchange instruction set
    blx r3

halt:
    @Wait for event
    @When the C function returns, it enters the halt procedure where it loops forever doing nothing.
    wfe
    b halt