SHELL := /bin/bash
HOST_CC = gcc
CROSS_PREFIX = /home/xim/Downloads/others/riscv64-unknown-elf-gcc-2018.07.0-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-
CC = ${CROSS_PREFIX}gcc
AR = ${CROSS_PREFIX}ar
USER_CFLAGS = -O0 -nostdlib -T user_riscv.lds -Wall -mcmodel=medany -Iinclude -Itest -Itiny_libc/include -nostdinc -g -fvar-tracking -Wl,--no-relax
USER_LDFLAGS = -L./ -ltiny_libc 

ARCH = riscv
ARCH_DIR = ./arch/$(ARCH)
# CFLAGS += -Iarch/$(ARCH)/include
USER_CFLAGS += -Iarch/$(ARCH)/include
USER_LDFLAGS += $(ARCH_DIR)/crt0.o

SRC_LIBC	= ./tiny_libc/printf.c ./tiny_libc/string.c ./tiny_libc/mthread.c ./tiny_libc/syscall.c ./tiny_libc/invoke_syscall.S \
				./tiny_libc/time.c ./tiny_libc/mailbox.c ./tiny_libc/rand.c ./tiny_libc/atol.c ./tiny_libc/atoi.c
SRC_LIBC_ASM	= $(filter %.S %.s,$(SRC_LIBC))
SRC_LIBC_C	= $(filter %.c,$(SRC_LIBC))

SRC_USER	= ./test/hello.elf

SRC_ELF2CHAR	= ./tools/elf2char.c
SRC_GENMAP	= ./tools/generateMapping.c

.PHONY:all clean

all: elf2char user

user: $(SRC_USER) elf2char generateMapping
	echo "" > user_programs.c
	echo "" > user_programs.h
	for prog in $(SRC_USER); do ./elf2char --header-only $$prog >> user_programs.h; done
	for prog in $(SRC_USER); do ./elf2char $$prog >> user_programs.c; done
	./generateMapping user_programs
	mv user_programs.h ../
	mv user_programs.c ../

libtiny_libc.a: $(SRC_LIBC_C) $(SRC_LIBC_ASM) user_riscv.lds
	for libobj in $(SRC_LIBC_C); do ${CC} ${USER_CFLAGS} -c $$libobj -o $${libobj/.c/.o}; done
	for libobj in $(SRC_LIBC_ASM); do ${CC} ${USER_CFLAGS} -c $$libobj -o $${libobj/.S/.o}; done
	${AR} rcs libtiny_libc.a $(patsubst %.c, %.o, $(patsubst %.S, %.o,$(SRC_LIBC)))

$(ARCH_DIR)/crt0.o: $(ARCH_DIR)/crt0.S
	${CC} ${USER_CFLAGS} -c $(ARCH_DIR)/crt0.S -o $(ARCH_DIR)/crt0.o

%.elf : %.c user_riscv.lds libtiny_libc.a $(ARCH_DIR)/crt0.o
	${CC} ${USER_CFLAGS} $< ${USER_LDFLAGS} -o $@

elf2char: $(SRC_ELF2CHAR)
	${HOST_CC} ${SRC_ELF2CHAR} -o elf2char -ggdb -Wall
generateMapping: $(SRC_GENMAP)
	${HOST_CC} ${SRC_GENMAP} -o generateMapping -ggdb -Wall


clean:
	rm -rf libtiny_libc.a
	rm ../user_programs.h ../user_programs.c
	find . -name "*.o" -exec rm {} \;

