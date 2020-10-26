/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_net.h"
#include <board.h>
#include "sbi.h"


static rt_err_t e1000_emac_init(rt_device_t dev)
{
    return RT_EOK;
}


/* Ethernet device interface */
/* transmit packet. */
static uint8_t tx_buf[2048];
rt_size_t e1000_emac_tx(rt_device_t dev,
                                    rt_off_t pos,
                                    const void* buffer,
                                    rt_size_t size)
{
    /* send it out */
    sbi_net_write(buffer, size * 1024);

    return 0;
}

/* reception packet. */
rt_size_t *e1000_emac_rx(rt_device_t dev,
                                   rt_off_t pos,
                                   void* buffer,
                                   rt_size_t size)
{
    volatile uint32_t *packet_len = (uint32_t *)(0x1e001000);
    char *packet_addr = (uint32_t *)(0x1e000000);

    while (*packet_len == 0) {
        sbi_net_read();
    }
    memcpy(buffer, packet_addr, *packet_len);
    *packet_len = 0;   
    
    return size;
}

static rt_err_t rt_net_control(rt_device_t dev, int cmd, void *args)
{
    uint32_t ui32Critical, i;
    uint32_t ui32CurrentPage, ui32CurrentBlock;

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL)
            return -RT_ERROR;

        geometry->bytes_per_sector = 1024;
        geometry->sector_count = 512;
        geometry->block_size = 1024;
    }

    else if(cmd == RT_DEVICE_CTRL_BLK_ERASE)
    {
        // do nothing here
    }

    return RT_EOK;
}

int rt_hw_net_init(void)
{
    static struct rt_device device;
    static struct rt_device_ops _device_ops;

    device.ops        = &_device_ops;

    /* register device */
    device.type        = RT_Device_Class_Block;
    _device_ops.init        = e1000_emac_init;
    _device_ops.open        = RT_NULL;
    _device_ops.close       = RT_NULL;
    _device_ops.read        = e1000_emac_rx;
    _device_ops.write       = e1000_emac_tx;
    _device_ops.control     = rt_net_control;

    /* no private */
    device.user_data = RT_NULL;

    /* register the device */
    rt_device_register(&device, "eth0", RT_DEVICE_FLAG_RDWR);

    //rt_kprintf("register device rom!\r\n");

    return 0;
}