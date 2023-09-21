# Raspberry_PI_OS_practice  ref:https://jsandler18.github.io/explanations/boot_S.html  
I'm using qemu-8.0.4(8.10 would fail while configuring) and gcc-arm-none-eabi-10.3-2021.10  
So the instructions should be  
"#./gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc -mcpu=cortex-a7 -fpic -ffreestanding -c boot.S -o boot.o#"  
"#./gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -c kernel.c -o kernel.o -O2 -Wall -Wextra#"  
"#./gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc -T linker.ld -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o#"  
After executing all the steps described in reference. To run codes in qemu, enter "*qemu-system-arm -m 1024 -M raspi2b -serial stdio -kernel myos.elf*"  
Since the machine listed in turtorial isn't showed up in this version of qemu(Use "*qemu-system-arm -machine help*" to look for supported machines)