/*
 * Copyright (c) 2020, Bamvor Jian ZHANG
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pthread.h>
#include <stdlib.h>
#include <sys/errno.h>

#include "metal-test.h"
#include <metal/atomic.h>
#include <metal/log.h>
#include <metal/sys.h>
#include "metal-test-internal.h"

static const int atomic_test_count = 10;

static void *atomic_thread(void *arg)
{
	atomic_int *c = arg;
	int i;


	metal_log(METAL_LOG_DEBUG, "%s enter: atomic test count %d\n", __func__,
		  atomic_test_count);
	for (i = 0; i < atomic_test_count; i++) {
		atomic_fetch_add(c, 1);
		metal_log(METAL_LOG_DEBUG, "counter %d\n", c);
	}

	return NULL;
}

int atomic(void)
{
	const int threads = 10;
	atomic_int counter = ATOMIC_VAR_INIT(0);
	int value, error;

	metal_log(METAL_LOG_DEBUG, "%s\n", __func__);
	error = metal_run(threads, atomic_thread, &counter);
	if (!error) {
		metal_log(METAL_LOG_DEBUG, "after metal_run\n");
		value = atomic_load(&counter);
		metal_log(METAL_LOG_DEBUG, "counter is %d after %d threads"
			  " execution\n", value, threads);
		value -= atomic_test_count * threads;
		if (value) {
			metal_log(METAL_LOG_DEBUG, "counter mismatch, delta = %d\n",
				  value);
			error = -EINVAL;
		}
	} else {
		metal_log(METAL_LOG_DEBUG, "metal_run failed\n");
	}

	return error;
}
