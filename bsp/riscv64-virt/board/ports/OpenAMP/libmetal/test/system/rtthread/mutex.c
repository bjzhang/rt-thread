/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "metal-test.h"
#include <metal/log.h>
#include <metal/sys.h>
#include <metal/mutex.h>
#include <rtthread.h>

static const int mutex_test_count = 1000;

void usleep(int tick)
{
    rt_thread_delay(tick);
}

static void *mutex_thread(void *arg)
{
    metal_mutex_t *l = arg;
    int i;

    for (i = 0; i < mutex_test_count; i++)
    {
        printf("before metal_mutex_acquire\n");
        metal_mutex_acquire(l);
        printf("before usleep\n");
        usleep(1);
        printf("before metal_mutex_release\n");
        metal_mutex_release(l);
	if (i % 100 == 0)
		printf("count: %d\n", i);
    }

    return NULL;
}

int mutex(void)
{
    metal_mutex_t lock;
    const int threads = 10;
    int rc;

    printf("mutex before usleep\n");
    usleep(1);
    printf("before metal_mutex_init\n");
    metal_mutex_init(&lock);
    printf("after metal_mutex_init\n");

    //rc = metal_run(threads, mutex_thread, &lock);
    mutex_thread(&lock);

    metal_mutex_deinit(&lock);

    return rc;
}
METAL_ADD_TEST(mutex);
