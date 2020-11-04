/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "metal-test.h"

int main(void)
{
	int status;

	printf("Hallo RT-Thread von RISC-V\n");
	status = metal_tests_run(NULL);
	printf("Bye RT-Thread on RISC-V\n");

	return status;
}
