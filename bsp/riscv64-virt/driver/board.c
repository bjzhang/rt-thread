/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "tick.h"

#include "drv_uart.h"
#include "drv_sd.h"
#include "drv_net.h"

#include "encoding.h"

#include "vm.h"

#include "stack.h"
#ifdef RT_USING_MMU
#include <stdio.h>
#include "syscall_number.h"
#endif
#ifdef RISCV_U_MODE
#include <string.h>
#include <pgtable.h>
#include <elf.h>
#include <vm.h>
#endif

void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

void primary_cpu_entry(void)
{
    extern void entry(void);

    /* disable global interrupt */
    init_bss();
    rt_hw_interrupt_disable();
    entry();
}

void rt_hw_interrupt_init() {
    // rt_hw_interrupt_enable(0);
    // FOO implementation here
    set_csr(sie, SIP_SSIP);
}
#define NUM_SYSCALLS 64
long (*syscall[NUM_SYSCALLS])();

void undef_syscall() {
    drv_uart_puts("Undefined syscall number\n");
}

void rt_syscall_init() {
    extern void rt_thread_exit();
    // init syscall table here
    for (int i = 0; i < NUM_SYSCALLS; ++i) syscall[i] = &undef_syscall;
    syscall[SYSCALL_WRITE] = &drv_uart_puts;
    syscall[SYSCALL_EXIT] = &rt_thread_exit;
}

#define PGDIR_PA (0x90200000UL + 24 * 1024 * 1024)
void rt_init_user_mem(struct rt_thread *thread, const char *name, unsigned long *entry) {
    clear_pgdir((uintptr_t)thread->pgdir);
    share_pgtable((uintptr_t)thread->pgdir, PGDIR_PA);
    unsigned char *elf_binary = NULL;
    int length                = 0;
    if (get_elf_file(name, &elf_binary, &length) == 0) {
        return RT_NULL;
    }
    *entry = (void *)load_elf(elf_binary, length, (uintptr_t)thread->pgdir,&alloc_page_helper);
    alloc_page_helper(USER_STACK_BASE, (uintptr_t)thread->pgdir); // kernel virtual address
    switch_pgdir((unsigned long)thread->pgdir);
}

void rt_hw_board_init(void)
{
    /* initialize MMU  */
    rt_vm_init();
#ifdef RT_USING_MMU
    extern rt_list_t freePageList;
    rt_syscall_init();
    rt_list_init(&freePageList);
#endif
    /* initalize interrupt */
    rt_hw_interrupt_init();
    /* initialize hardware interrupt */
    rt_hw_uart_init();
    rt_hw_sd_init();
    //rt_hw_net_init();
    //rt_device_t d;
    rt_hw_net_init();
    

    rt_hw_tick_init();

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device("uart\0\0\0");
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_HEAP
    rt_kprintf("heap: [0x%08x - 0x%08x]\n", (rt_ubase_t) RT_HW_HEAP_BEGIN, (rt_ubase_t) RT_HW_HEAP_END);
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
void rt_hw_cpu_reset(void)
{
    // sysctl->soft_reset.soft_reset = 1;
    while(1);
}

MSH_CMD_EXPORT_ALIAS(rt_hw_cpu_reset, reboot, reset machine);

extern struct rt_serial_device  serial1;


#define SYSCALL_SCAUSE 8
void handle_syscall(uintptr_t scause, uintptr_t sepc, uintptr_t sp) {
    struct rt_hw_stack_frame* regs = (struct rt_hw_stack_frame *)sp;
    uint64_t syscall_number = regs->a7;
    uint64_t ret = syscall[regs->a7](regs->a0, regs->a1, regs->a2);
    if (syscall_number != SYSCALL_EXIT) {
        regs->a0 = ret;
        regs->epc = regs->epc + 4; // jump over this
    }
}

static rt_isr_handler_t softirq_handler = RT_NULL;
rt_isr_handler_t rt_hw_irq_soft_install(rt_isr_handler_t handler)
{
    rt_isr_handler_t old_handler = softirq_handler;

    if (handler != RT_NULL)
	    softirq_handler = handler;

    return old_handler;
}

void handle_irq_soft(void)
{
    if (softirq_handler != RT_NULL)
	    softirq_handler(0, RT_NULL);
}

uintptr_t handle_trap(uintptr_t scause, uintptr_t stval, uintptr_t sepc, uintptr_t sp) {
    // while (1);
    rt_hw_interrupt_disable();
    if (scause == (uint64_t)(0x8000000000000005)) {
        // timer interrupt
        tick_isr();
    }
    if (scause == (uint64_t)(0x8000000000000001)) {
        // software interrupt
       clear_csr(sip, SIP_SSIP);
       handle_irq_soft();
    }
    if (scause != SYSCALL_SCAUSE) {
        if (serial1.serial_rx != NULL)
            rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND); // try grabing some data
        // not a syscall
        rt_schedule();
    } else {
        // syscall
        handle_syscall(scause, sepc, sp);
    }
    return 0;
}
