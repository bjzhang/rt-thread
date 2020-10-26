/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "drv_uart.h"

#include <stdio.h>
#include "sbi.h"


#define UART_DEFAULT_BAUDRATE               115200

struct device_uart
{
    rt_uint32_t hw_base;
    rt_uint32_t irqno;
};

static rt_err_t  rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg);
static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg);
static int       drv_uart_putc(struct rt_serial_device *serial, char c);
static int       drv_uart_getc(struct rt_serial_device *serial);

const struct rt_uart_ops _uart_ops =
{
    rt_uart_configure,
    uart_control,
    drv_uart_putc,
    drv_uart_getc,
    //TODO: add DMA support
    RT_NULL
};


void uart_init(void)
{
    // foo implementation
}

struct rt_serial_device  serial1;
struct device_uart       uart1;

uint32_t buffer0[256] = {
    0xffffffff, 0x5500ffff, 0xf77db57d, 0x00450008, 0x0000d400, 0x11ff0040,
    0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0004e914, 0x0000,     0x005e0001,
    0x2300fb00, 0x84b7f28b, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8,
    0x00e00101, 0xe914fb00, 0x0801e914, 0x0000};

/*
 * UART Initiation
 */
int rt_hw_uart_init(void)
{
    struct rt_serial_device *serial;
    struct device_uart      *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    {
        

        serial  = &serial1;
        uart    = &uart1;

        serial->ops              = &_uart_ops;
        serial->config           = config;
        serial->config.baud_rate = UART_DEFAULT_BAUDRATE;

        uart->hw_base   = 0; // FOO implementation
        uart->irqno     = 0; // FOO implementation
        

        // uart_init(UART_DEVICE_1);

        rt_hw_serial_register(serial,
                              "uart",
                              RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              uart);
    }
    sbi_net_write(buffer0, 88);

    return 0;
}

/*
 * UART interface
 */

static rt_err_t rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct device_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    serial->config = *cfg;

    return (RT_EOK);
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct device_uart *uart;

    uart = serial->parent.user_data;
    rt_uint32_t channel = 1;

    RT_ASSERT(uart != RT_NULL);
    RT_ASSERT(channel != 3);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* Disable the UART Interrupt */
        //rt_hw_interrupt_mask(uart->irqno);
        // _uart[channel]->IER &= ~0x1;
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* install interrupt */
        // rt_hw_interrupt_install(uart->irqno, uart_irq_handler,
        //                        serial, serial->parent.parent.name);
        // rt_hw_interrupt_umask(uart->irqno);
        //_uart[channel]->IER |= 0x1;
        break;
    }

    return (RT_EOK);
}

static int drv_uart_putc(struct rt_serial_device *serial, char c)
{
    // using SBI interface
    SBI_CALL_1(SBI_CONSOLE_PUTCHAR, c);
    return (1);
}

static int drv_uart_getc(struct rt_serial_device *serial)
{
    // using SBI interface
    return SBI_CALL_0(SBI_CONSOLE_GETCHAR);
}

void drv_uart_puts(char *str) {
    sbi_console_putstr(str);
}

char rt_hw_console_getchar(void) {
    return SBI_CALL_0(SBI_CONSOLE_GETCHAR);
}


/* WEAK for SDK 0.5.6 */

RT_WEAK void uart_debug_init(int uart_channel)
{

}

