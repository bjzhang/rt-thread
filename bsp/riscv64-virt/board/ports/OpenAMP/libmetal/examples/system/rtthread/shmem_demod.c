/*
 * Copyright (c) 2017, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 /*****************************************************************************
  * shmem_demod.c
  * This demo demonstrates the use of shared mem. between the APU and RPU.
  * This demo does so via the following steps:
  *
  *  1. Get the shared memory device I/O region.
  *  2. Clear the demo control value in shared memory.
  *  3. Check the demo control value in the shared memory to wait for APU
  *     to start the demo.
  *  4. Once the demo control value indicates the demo starts, it polls on
  *     RX available value to see if there is new RX message available.
  *  5. If there is a new RX message available, it reads the message from
  *     the shared memory
  *  6. It echos back the message to the shared memory
  *  7. It increases the TX available value in the shared memory to notify
  *     the other end there is a message available to read.
  *  8. Check if the demo control value and the RX available values to see
  *     if demo finishes and if there is new RX data available.
  *
  * Here is the Shared memory structure of this demo:
  * |0      | 4Bytes | DEMO control status shows if demo starts or not |
  * |0x04 | 4Bytes | number of APU to RPU buffers available to RPU |
  * |0x08 | 4Bytes | number of APU to RPU buffers consumed by RPU |
  * |0x0c | 4Bytes | number of RPU to APU buffers available to APU |
  * |0x10 | 4Bytes | number of RPU to APU buffers consumed by APU |
  * |0x14 | 1KBytes | APU to RPU buffer |
  * ... ...
  * |0x800 | 1KBytes | RPU to APU buffer |
  */

#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/alloc.h>

/* Shared memory offsets */
#define SHM_DEMO_CNTRL_OFFSET       0x0
#define SHM_RX_AVAIL_OFFSET        0x04
#define SHM_RX_USED_OFFSET         0x08
#define SHM_TX_AVAIL_OFFSET        0x0C
#define SHM_TX_USED_OFFSET         0x10
#define SHM_RX_BUFFER_OFFSET       0x14
#define SHM_TX_BUFFER_OFFSET       0x800

#define SHM_BUFFER_SIZE          0x400

#define DEMO_STATUS_IDLE         0x0
#define DEMO_STATUS_START        0x1 /* Status value to indicate demo start */

struct msg_hdr_s {
	uint32_t index;
	uint32_t len;
};

#define LPRINTF(format, ...) \
	        rt_kprintf("\r\nCLIENT> " format, ##__VA_ARGS__)

#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)


#define VRING_START (0x90000000)
#define VRING_SIZE  (0x00100000)

#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))

#define VRING(io, i)  _REG32(io->virt, i)
/**
 * @brief shmem_echod() - Show use of shared memory with libmetal.
 *        Wait for message from APU. Once received, read and echo it back.
 *
 * @param[in] shm_io - metal i/o region of the shared memory
 * @return - return 0 on success, otherwise return error number indicating
 *           type of error
 */
static int shmem_echod(struct metal_io_region *shm_io)
{
	uint32_t data_32;

	LPRINTF("Demo started\n");
	//LPRINTF("Wait for shared memory demo to start.\r\n");
	metal_io_write32(shm_io, SHM_DEMO_CNTRL_OFFSET, 0);
	while((data_32 = metal_io_read32(shm_io, SHM_DEMO_CNTRL_OFFSET)) == 0);
	LPRINTF("Got 0x%lx from 0x%lx\n", data_32, SHM_DEMO_CNTRL_OFFSET);
//	VRING(shm_io, 0) = 0x1;
//	VRING(shm_io, 4) = 0x2;
//	VRING(shm_io, 8) = 0x3;
//	VRING(shm_io, 0xc) = 0x4;
//	LPRINTF("Wait finish\r\n");
//	while(VRING(shm_io, 0x10) == 0);
//	LPRINTF("Shared memory test finished\r\n");
	return 0;
}

int shmem_demod()
{
	struct metal_io_region *io = NULL;
	struct metal_io_region vring;
	int ret = 0;

	LPRINTF("Configuration share memory\n");
	memset(&vring, 0, sizeof(vring));
	io = &vring;
	metal_io_init(io, (void*)VRING_START, (const metal_phys_addr_t *)VRING_START, VRING_SIZE, -1, 0, NULL);

#if 0
	/* Get shared memory device IO region */
	if (!shm_dev) {
		ret = -ENODEV;
		goto out;
	}
	io = metal_device_io_region(shm_dev, 0);
	if (!io) {
		LPERROR("Failed to map io region for %s.\n", shm_dev->name);
		ret = -ENODEV;
		goto out;
	}
#endif
	ret = shmem_echod(io);

out:
	return ret;

}
