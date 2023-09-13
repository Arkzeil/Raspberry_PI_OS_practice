# Raspberry_PI_OS_practice  ref:https://jsandler18.github.io/explanations/boot_S.html  
I'm using qemu-8.0.4(8.10 would fail while configuring) and gcc-arm-none-eabi-10.3-2021.10  
After executing all the steps described in reference. To run codes in qemu, enter "*qemu-system-arm -m 1024 -M raspi2b -serial stdio -kernel myos.elf*"  
Since the machine listed in turtorial isn't showed up in this version of qemu(Using *qemu-system-arm -machine help* to look up supported ones)