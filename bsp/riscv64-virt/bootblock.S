/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
 
/*
 * Please note that this file is NOT compiled with ANY automatic scripts
 * I put it here to make sure that if any one is interested in the bootblock,
 * they can get a proper reference.
 *
 * If you DO NEED compile it, use the bootblock.lds as the linker script
 * the required SBI headers are located in driver/asm
 */
#include <asm/sbidef.h>
#include <asm/sbiasm.h>

// #define BOOT_LOADER_SIG_OFFSET 0x1fe
// #define OS_SIZE_LOC (BOOT_LOADER_SIG_OFFSET - 2)
// #define OS_SIZE_LOC 0x1fc
#define KERNEL_STACK	0x10500000
.equ os_size_loc, 0x102001fc

// 1. kernel address (move kernel to here ~)
.equ kernel, 0x10201000

// 2. kernel main address (jmp here to start kernel main!)
.equ kernel_main, 0x10201000

.text
.global main

main:
	// fence on all memory and I/O
	fence

  // save FDT
  move s0,a0
  move s1,a1

	// output a \n
	li a0, 10
	SBI_CALL SBI_CONSOLE_PUTCHAR

	// 1) task1 call BIOS print string "It's bootblock!"
    la a0, msg
	SBI_CALL SBI_CONSOLE_PUTSTR

	/* 2) task2 call BIOS read kernel in SD card and jump to kernel start */

	/* read kernel in SD card, you should read `os_size` sectors */
	/* TODO: */
	

copy_to_dest_start:
	
	/* num_of_blocks */
	lui a0,	%hi(os_size_loc)
	addi a0, a0, %lo(os_size_loc)
	lh a1,	0(a0)
	/* store the size of kernel in s6 */
	addi s6, a1, 0
	/* the offset of kernel load address in s7 */
	li s7, 0
	/* add s7 by 0x8000 every loop */
	/* the offset of sdcard blocks in s8, increase by 64 every loop */
	li s8, 0
	li s9, 64
	li s10, 0x8000
load_loop:
	/* start physical address of kernel */
	lui a0, %hi(kernel)
	addi a0, a0, %lo(kernel)
	add a0, a0, s7

	/* start block in SD card */
	li a2, 1
	add a2, a2, s8

	bge s6, s9, skip
	move a1, s6 // a1 = s6 = the left size of kernel
	j next
skip:
	li a1, 64
next:
	/* launch the read process */
	SBI_CALL SBI_SD_READ

	addi s6, s6, -64
	add s7, s7, s10
	addi s8, s8, 64
	bgt s6, zero, load_loop

	// output a \n
	li a0, 10
	SBI_CALL SBI_CONSOLE_PUTCHAR
    /* flush instruction cache */
	fence
	fence.i
    // pass fdt's address by $a1
    move a0, s0
    move a1, s1
	/* jump to main */
	lui t0, %hi(kernel_main)
	addi t0, t0, %lo(kernel_main)
	jalr t0
	// here we go!

// while(1) --> stop here
stop:
	j stop
	nop
copy_to_dest_end:

.data

msg: .string "It's a bootloader...\n\r"
