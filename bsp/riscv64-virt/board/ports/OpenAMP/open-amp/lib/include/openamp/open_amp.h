/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef OPEN_AMP_H_
#define OPEN_AMP_H_

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_virtio.h>
//OPENAMP_RPROC: make use of remote proc
#ifdef OPENAMP_RPROC
#include <openamp/remoteproc.h>
#include <openamp/remoteproc_virtio.h>
#endif /* #ifdef OPENAMP_RPROC */

#endif				/* OPEN_AMP_H_ */
