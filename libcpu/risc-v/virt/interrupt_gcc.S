/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/02     Bernard      The first version
 * 2018/12/27     Jesven       Add SMP schedule
 */

#include "cpuport.h"
#include "encoding.h"

  .section      .text.entry
  .align 2
  .global trap_entry
trap_entry:

    /* save thread context to thread stack */
    addi sp, sp, -32 * REGBYTES

    STORE x1,   1 * REGBYTES(sp)

    csrr  x1, sstatus
    STORE x1,   2 * REGBYTES(sp)

    csrr  x1, sepc
    STORE x1, 0 * REGBYTES(sp)

    STORE x4,   4 * REGBYTES(sp)
    STORE x5,   5 * REGBYTES(sp)
    STORE x6,   6 * REGBYTES(sp)
    STORE x7,   7 * REGBYTES(sp)
    STORE x8,   8 * REGBYTES(sp)
    STORE x9,   9 * REGBYTES(sp)
    STORE x10, 10 * REGBYTES(sp)
    STORE x11, 11 * REGBYTES(sp)
    STORE x12, 12 * REGBYTES(sp)
    STORE x13, 13 * REGBYTES(sp)
    STORE x14, 14 * REGBYTES(sp)
    STORE x15, 15 * REGBYTES(sp)
    STORE x16, 16 * REGBYTES(sp)
    STORE x17, 17 * REGBYTES(sp)
    STORE x18, 18 * REGBYTES(sp)
    STORE x19, 19 * REGBYTES(sp)
    STORE x20, 20 * REGBYTES(sp)
    STORE x21, 21 * REGBYTES(sp)
    STORE x22, 22 * REGBYTES(sp)
    STORE x23, 23 * REGBYTES(sp)
    STORE x24, 24 * REGBYTES(sp)
    STORE x25, 25 * REGBYTES(sp)
    STORE x26, 26 * REGBYTES(sp)
    STORE x27, 27 * REGBYTES(sp)
    STORE x28, 28 * REGBYTES(sp)
    STORE x29, 29 * REGBYTES(sp)
    STORE x30, 30 * REGBYTES(sp)
    STORE x31, 31 * REGBYTES(sp)

    /* switch to interrupt stack */
    move  s0, sp
    
    lui s1, %hi(0xFFFFFFC0ULL)
    addi s1, s1, %lo(0xFFFFFFC0ULL)
    slli s1, s1, 32
    bgtu sp, s1, skip_switch_kernel_stack
    // for a userland process, we reuse user stack
switch_kernel_stack:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* switch interrupt stack of current cpu */
    la    sp, __stack_start__
    li    t2, __STACKSIZE__
    add   sp, sp, t2 
    /* sp = 2 * __STACKSIZE__ + __stack_start__ */

skip_switch_kernel_stack:

    /* handle interrupt */
    call  rt_interrupt_enter

    csrr a0, scause
    csrrc a1, stval, zero
    csrr  a2, sepc
    mv    a3, s0

    /* scause, stval, sepc, sp */
    call  handle_trap
    call  rt_interrupt_leave


    /* switch to from_thread stack */
    move  sp, s0

    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, spurious_interrupt
    sw    zero, 0(s0)

    la    s0, rt_interrupt_from_thread
    LOAD  s1, 0(s0)
    STORE sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    LOAD  s1, 0(s0)
    LOAD  sp, 0(s1)


spurious_interrupt:
    j rt_hw_context_switch_exit


.global rt_hw_interrupt_enable
rt_hw_interrupt_enable:
    csrs CSR_SSTATUS, a0
    jr ra

.global rt_hw_interrupt_disable
rt_hw_interrupt_disable:
    csrrci a0, sstatus, 2
    jr ra
