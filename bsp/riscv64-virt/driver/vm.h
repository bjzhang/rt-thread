/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef VM_H
#define VM_H
void rt_vm_init(void);
uintptr_t alloc_page_helper(uintptr_t va, uintptr_t pgdir);
void switch_pgdir(unsigned long pgdir);
void rt_cleanup_pgtable(void* pgdir);
uintptr_t allocPage();
void share_pgtable(uintptr_t dest_pgdir, uintptr_t src_pgdir);
#define USER_STACK_BASE 0xFFFFFFC000000000ULL
#endif