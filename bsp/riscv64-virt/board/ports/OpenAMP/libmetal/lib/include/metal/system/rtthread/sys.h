/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 * Copyright (c) 2021, Bamvor Jian Zhang. RTT. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	rtthread/riscv/sys.h
 * @brief	rtthread riscv system primitives for libmetal.
 */

#ifndef __METAL_SYS__H__
#error "Include metal/sys.h instead of metal/rtthread/@PROJECT_MACHINE@/sys.h"
#endif

#ifndef __METAL_RTTHREAD_RISCV_SYS__H__
#define __METAL_RTTHREAD_RISCV_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

/** Structure of generic libmetal runtime state. */
struct metal_state {

	/** Common (system independent) data. */
	struct metal_common_state common;
};

/**
 * @brief	metal_rv64_virt_irq_isr
 *
 * riscv64 virt interrupt ISR can be registered to the Xilinx embeddedsw
 * IRQ controller driver.
 *
 * @param[in] arg input argument, interrupt vector id.
 */
void metal_rv64_virt_irq_isr(void *arg);

/**
 * @brief	metal_rv64_virt_irq_int
 *
 * riscv64 virt interrupt controller initialization. It will initialize
 * the metal Xilinx IRQ controller data structure.
 *
 * @return 0 for success, or negative value for failure
 */
int metal_rv64_virt_irq_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __METAL_RTTHREAD_RISCV_SYS__H__ */
