/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <rtthread.h>

#include "metal-test.h"

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

/* 线程1控制块 */
static struct rt_thread thread1;
/* 线程1栈 */
static rt_uint8_t thread1_stack[THREAD_STACK_SIZE];
/* 线程2控制块 */
static struct rt_thread thread2;
/* 线程2栈 */
static rt_uint8_t thread2_stack[THREAD_STACK_SIZE];

/* 线程入口 */
static void thread_entry(void* parameter)
{
    rt_uint32_t count = 0;
    rt_uint32_t no = (rt_uint32_t) parameter; /* 获得正确的入口参数 */

    while (1)
    {
        /* 打印线程计数值输出 */
        rt_kprintf("thread%d count: %d\n", no, count ++);

//        /* 休眠10个OS Tick */
//        rt_thread_delay(10);
    }
}

int thread_static_simple()
{
    rt_err_t result;

    /* 初始化线程1 */
    result = rt_thread_init(&thread1, "t1", /* 线程名：t1 */
        thread_entry, (void*)1, /* 线程的入口是thread_entry，入口参数是1 */
        &thread1_stack[0], sizeof(thread1_stack), /* 线程栈是thread1_stack */
        THREAD_PRIORITY, 10);
    if (result == RT_EOK) /* 如果返回正确，启动线程1 */
        rt_thread_startup(&thread1);
    else
        printf("TC_STAT_END | TC_STAT_FAILED");

    /* 初始化线程2 */
    result = rt_thread_init(&thread2, "t2", /* 线程名：t2 */
        thread_entry, RT_NULL, /* 线程的入口是thread_entry，入口参数是2 */
        &thread2_stack[0], sizeof(thread2_stack), /* 线程栈是thread2_stack */
        THREAD_PRIORITY + 1, 10);
    if (result == RT_EOK) /* 如果返回正确，启动线程2 */
        rt_thread_startup(&thread2);
    else
        printf("TC_STAT_END | TC_STAT_FAILED");

    rt_thread_delay(100);
    printf("%s return\n", __func__);
    return 0;
}
FINSH_FUNCTION_EXPORT(thread_static_simple, thread example);
MSH_CMD_EXPORT(thread_static_simple, thread example);

int main(void)
{
	int status;

	rt_kprintf("Hello RT-Thread on RISC-V\n");

	return status;
}

