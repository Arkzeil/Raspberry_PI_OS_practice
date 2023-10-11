# ref : https://jsandler18.github.io/tutorial/organize.html

# Use cross compiler
CC = ./gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc

# make the Model 2 stuff default, and have to trigger the Model 1 stuff manually by passing RASPI_MODEL=1 as a parameter to 'make'
ifeq ($(RASPI_MODEL), 1)
	CPU = arm1176jzf.s
	DIRECTIVES = -D MODEL_1
	ARCHDIR = model1
else
    CPU = cortex-a7
	ARCHDIR = model2
endif

#variables for comiler and linker
CFLAGS= -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES)
CSRCFLAGS= -O2 -Wall -Wextra
LFLAGS= -ffreestanding -O2 -nostdlib

# Location of the files
KER_SRC = ./src/kernel
KER_HEAD = ./include
COMMON_SRC = ./src/common
OBJ_DIR = objects
KERSOURCES += $(wildcard $(KER_SRC)/$(ARCHDIR)/*.c)
COMMONSOURCES = $(wildcard $(COMMON_SRC)/*.c)
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)
# $(patsubst <pattern>, <replacement>, <text>) which would replace last matched in "%...", seperated by space
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(COMMON_SRC)/%.c, $(OBJ_DIR)/%.o, $(COMMONSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))
HEADERS = $(wildcard $(KER_HEAD)/*.h)

IMG_NAME = kernel.img

# depends on all of the object files for the respective source code files, and all the header files.
# Meaning that all the object files must be compiled before this can execute. Its only command is to link all of the objects together into the final kernel binary.
build: $(OBJECTS) $(HEADERS)
	echo $(OBJECTS)
	$(CC) -T linker.ld -o $(IMG_NAME) $(LFLAGS) $(OBJECTS)

# '$(@D)' is the directory of '$@'(target file)
# https://jasonblog.github.io/note/gunmake/makefile_zhong_de_,_%5E,__,__fu_hao.html
$(OBJ_DIR)/%.o: $(KER_SRC)/$(ARCHDIR)/%.c # target : dependency
	mkdir -p $(@D) # make folder if not existed
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS) # -I allows source files to access include files by #include <kernel/header.h> instead of #include <../../include/kernel/header/h>

$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -c $< -o $@

$(OBJ_DIR)/%.o: $(COMMON_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KER_SRC) -I$(KER_HEAD) -c $< -o $@ $(CSRCFLAGS)

clean:
	rm -rf $(OBJ_DIR)
	rm $(IMG_NAME)

run: build
	qemu-system-arm -m 1024 -M raspi2b -serial stdio -kernel kernel.img
    #qemu-system-arm -m 256 -M raspi2 -serial stdio -kernel kernel.img