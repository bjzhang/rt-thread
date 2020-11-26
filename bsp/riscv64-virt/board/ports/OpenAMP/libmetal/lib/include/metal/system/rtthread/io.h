/*
 * Copyright (c) 2017, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/io.h
 * @brief	Generic specific io definitions.
 */

#ifndef __METAL_IO__H__
#error "Include metal/io.h instead of metal/rtthread/io.h"
#endif

#ifndef __METAL_RTTHREAD_IO__H__
#define __METAL_RTTHREAD_IO__H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL

/**
 * @brief memory mapping for an I/O region
 */
void metal_sys_io_mem_map(struct metal_io_region *io);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_RTTHREAD_IO__H__ */
