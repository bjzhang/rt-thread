/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	rtthread/sys.h
 * @brief	FreeRTOS system primitives for libmetal.
 */

#ifndef __METAL_SYS__H__
#error "Include metal/sys.h instead of metal/rtthread/sys.h"
#endif

#ifndef __METAL_RTTHREAD_SYS__H__
#define __METAL_RTTHREAD_SYS__H__

//#include "./@PROJECT_MACHINE@/sys.h"

/** Structure for rt-thread libmetal runtime state. */
struct metal_state {

	/** Common (system independent) data. */
	struct metal_common_state common;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_RTTHREAD_SYS__H__ */
