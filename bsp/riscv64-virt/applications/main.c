/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-30     Tanek        first version
 */

#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <dfs_fs.h>

int main(void)
{
	// printf("Hallo RT-Thread von RISC-V\n");
    // rt_thread_create()
    rt_thread_t tid;
    tid = rt_thread_create("hello", (void (*)(void *parameter))0xFFFFFFE000000000ULL, RT_NULL,
                           4096, 10, 20);
    rt_thread_startup(tid);
    if (dfs_mount("sdcard", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    } else {
        rt_kprintf("Error mounting rootfs\n");
    }
	return 0;
}
