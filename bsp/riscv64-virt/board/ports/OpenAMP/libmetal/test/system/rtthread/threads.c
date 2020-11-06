/*
 * Copyright (c) 2020, Bamvor Jian ZHANG
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/sys.h>
#include <metal/utilities.h>
#include "metal-test.h"
#include <rtdef.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

int metal_run(int threads, metal_thread_t child, void *arg)
{
    rt_thread_t tids[threads];
    int error, ts_created;

    error = metal_run_noblock(threads, child, arg, tids, &ts_created);
    metal_log(METAL_LOG_ERROR, "metal_run_noblock return %d\n", error);

    metal_finish_threads(ts_created, (void *)tids);

    return error;
}

int metal_run_noblock(int threads, metal_thread_t child,
                      void *arg, void *tids, int *threads_out)
{
    rt_thread_t *tid_p = (rt_thread_t *)tids;
    char name[sizeof("thread") + sizeof(typeof(threads))];
    int i;
    int error = 0;

    if (!tids)
    {
        metal_log(METAL_LOG_ERROR, "invalid arguement, tids is NULL.\n");
        return -EINVAL;
    }
    memset(name, 0, sizeof(name));
    for (i = 0; i < threads; i++)
    {
        sprintf(name, "thread%00x", i);
        tid_p[i] = rt_thread_create(name, child, RT_NULL,
                                     THREAD_STACK_SIZE, THREAD_PRIORITY,
                                     THREAD_TIMESLICE);
        if (tid_p[i] != RT_NULL)
        {
            metal_log(METAL_LOG_DEBUG, "create ok\n");
            rt_thread_startup(tid_p[i]);
            metal_log(METAL_LOG_DEBUG, "rt_thread_startup return\n");
        }
        else
        {
            error--;
            metal_log(METAL_LOG_ERROR, "failed to create thread\n");
            break;
        }
    }

    *threads_out = i;
    metal_log(METAL_LOG_DEBUG, "thread create number %d, error %d\n", *threads_out, error);
    return error;
}

void metal_finish_threads(int threads, void *tids)
{
    int i;
    rt_thread_t *tid_p = (rt_thread_t *)tids;

    metal_log(METAL_LOG_DEBUG, "%s\n", __func__);
    if (!tids)
    {
        metal_log(METAL_LOG_ERROR, "invalid argument, tids is NULL.\n");
        return;
    }

    for (i = 0; i < threads; i++)
        (void)rt_thread_delete(tid_p[i]);
}

