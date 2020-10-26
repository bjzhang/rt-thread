/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_sd.h"
#include "sbi.h"

/* RT-Thread device interface */
static rt_err_t rt_sd_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_sd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_sd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_sd_control(rt_device_t dev, int cmd, void *args)
{
    uint32_t ui32Critical, i;
    uint32_t ui32CurrentPage, ui32CurrentBlock;

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL)
            return -RT_ERROR;

        geometry->bytes_per_sector = 512; // 512B
        geometry->sector_count = 512;
        geometry->block_size = 512;
    }

    else if(cmd == RT_DEVICE_CTRL_BLK_ERASE)
    {
        // do nothing here
    }

    return RT_EOK;
}
#define SECTOR_BASE 1024
static rt_size_t rt_sd_read(rt_device_t dev,
                                   rt_off_t pos,
                                   void* buffer,
                                   rt_size_t size)
{
    
    //RT_ASSERT(size % 512 == 0);
    rt_size_t read_sz, ori = size;
    while (size > 0) {
        read_sz = (size > 64) ? 64 : size;
        sbi_sd_read(buffer, read_sz, SECTOR_BASE + pos);
        size -= read_sz;
        pos += read_sz;
        buffer = ((char *)buffer) + read_sz * 512;
    }
    
    //for (int i = 0; i < size; ++i) {
    //    ((char *)buffer)[i] = sd_ram[pos + i];
    //}
    return ori;
}

static rt_size_t rt_sd_write(rt_device_t dev,
                                    rt_off_t pos,
                                    const void* buffer,
                                    rt_size_t size)
{
    //RT_ASSERT(size % 512 == 0);

    rt_size_t write_sz, ori = size;
    while (size > 0) {
        write_sz = (size > 64) ? 64 : size;
        sbi_sd_write(buffer, write_sz, SECTOR_BASE + pos);
        size -= write_sz;
        pos += write_sz;
        buffer = ((char *)buffer) + write_sz * 512;
    }
    
    //for (int i = 0; i < size; ++i) {
    //    sd_ram[pos + i] = ((char *)buffer)[i];
    //}
    return ori;
}

int rt_hw_sd_init(void)
{
    static struct rt_device device;
    static struct rt_device_ops _device_ops;

    device.ops        = &_device_ops;

    /* register device */
    device.type        = RT_Device_Class_Block;
    _device_ops.init        = rt_sd_init;
    _device_ops.open        = rt_sd_open;
    _device_ops.close       = rt_sd_close;
    _device_ops.read        = rt_sd_read;
    _device_ops.write       = rt_sd_write;
    _device_ops.control     = rt_sd_control;

    /* no private */
    device.user_data = RT_NULL;

    /* register the device */
    rt_device_register(&device, "sdcard", RT_DEVICE_FLAG_RDWR);

    //rt_kprintf("register device rom!\r\n");

    return 0;
}