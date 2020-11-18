/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/alloc.c
 * @brief	generic libmetal memory allocattion definitions.
 */

#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/rtthread/alloc.h"
#endif

#ifndef __METAL_RTTHREAD_ALLOC__H__
#define __METAL_RTTHREAD_ALLOC__H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void *metal_allocate_memory(unsigned int size)
{
	return rt_malloc(size);
}

static inline void metal_free_memory(void *ptr)
{
	rt_free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_RTTHREAD_ALLOC__H__ */
