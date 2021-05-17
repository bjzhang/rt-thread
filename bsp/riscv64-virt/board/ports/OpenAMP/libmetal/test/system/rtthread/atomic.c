/*
 * Copyright (c) 2020, Bamvor Jian ZHANG
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <sys/errno.h>

#include "metal-test.h"
#include <metal/atomic.h>
#include <metal/log.h>
#include <metal/sys.h>
#include "metal-test-internal.h"
#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        10

static const int atomic_test_count = 10;

static void atomic_thread(void *arg)
{
	atomic_int *c = arg;
	int i;


	metal_log(METAL_LOG_DEBUG, "%s enter\n", __func__);
//	metal_log(METAL_LOG_DEBUG, "%s enter: atomic test count %d\n", __func__,
//		  atomic_test_count);
//	for (i = 0; i < atomic_test_count; i++) {
//		printf("counter before add %d\n", c);
//		atomic_fetch_add(c, 1);
////		metal_log(METAL_LOG_DEBUG, "counter %d\n", c);
//		printf("counter %d\n", c);
//	}
	metal_log(METAL_LOG_DEBUG, "%s exit\n", __func__);
}

int atomic(void)
{
	const int threads = 1;
	atomic_int counter = ATOMIC_VAR_INIT(0);
	int value, error;
	rt_thread_t thread = NULL;

	metal_log(METAL_LOG_DEBUG, "%s\n", __func__);
	metal_log(METAL_LOG_DEBUG, "delay test start\n");
	rt_thread_delay(200);
	metal_log(METAL_LOG_DEBUG, "delay test end\n");
	thread = rt_thread_create("atomic", atomic_thread, RT_NULL,
				  THREAD_STACK_SIZE,
				  THREAD_PRIORITY,
				  THREAD_TIMESLICE);
        if (thread != RT_NULL)
        {
            metal_log(METAL_LOG_DEBUG, "create ok\n");
            rt_thread_startup(thread);
            metal_log(METAL_LOG_DEBUG, "rt_thread_startup return\n");
        }
        else
        {
            metal_log(METAL_LOG_ERROR, "failed to create thread\n");
	    error = -1;
	    goto exit;
        }
	metal_log(METAL_LOG_DEBUG, "counter %d\n", counter);
	atomic_fetch_add(&counter, 1);
	metal_log(METAL_LOG_DEBUG, "counter %d\n", counter);
//	if (!error) {
//		metal_log(METAL_LOG_DEBUG, "after metal_run\n");
//		value = atomic_load(&counter);
//		metal_log(METAL_LOG_DEBUG, "counter is %d after %d threads"
//			  " execution\n", value, threads);
//		value -= atomic_test_count * threads;
//		if (value) {
//			metal_log(METAL_LOG_DEBUG, "counter mismatch, delta = %d\n",
//				  value);
//			error = -EINVAL;
//		}
//	} else {
//		metal_log(METAL_LOG_DEBUG, "metal_run failed\n");
//	}

	error = 0;
exit:
	return error;
}
