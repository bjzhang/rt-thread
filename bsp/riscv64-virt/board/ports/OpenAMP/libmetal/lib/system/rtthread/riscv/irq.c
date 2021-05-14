/*
 * Copyright (c) 2016 - 2017, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	rtthread/riscv/irq.c
 * @brief	rt-thread libmetal Xilinx irq controller definitions.
 */

#include <errno.h>
#include <metal/irq_controller.h>
#include <metal/sys.h>
#include <metal/log.h>
#include <metal/mutex.h>
#include <metal/list.h>
#include <metal/utilities.h>
#include <metal/alloc.h>

//only one software interrupt is supported right now
#define MAX_IRQS 1

static struct metal_irq irqs[MAX_IRQS];

static void metal_rv64_virt_irq_set_enable(struct metal_irq_controller *irq_cntr,
					   int irq, unsigned int state)
{
    (void)irq_cntr;
    (void)irq;
    (void)state;
    return;
}

static METAL_IRQ_CONTROLLER_DECLARE(rv64_virt_irq_cntr,
				    0, 1,
				    NULL,
				    metal_rv64_virt_irq_set_enable, NULL,
				    irqs);

void metal_rv64_virt_irq_isr(void *arg)
{
    unsigned int vector;

    vector = (uintptr_t)arg;
    if (vector >= MAX_IRQS) {
        return;
    }
    (void)metal_irq_handle(&irqs[vector], (int)vector);

}

int metal_rv64_virt_irq_init(void)
{
    int ret;

    ret = metal_irq_register_controller(&rv64_virt_irq_cntr);
    if (ret < 0) {
        metal_log(METAL_LOG_ERROR, "%s: register irq controller failed.\n",
                  __func__);
        return ret;
    }
    return 0;
}

