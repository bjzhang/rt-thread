/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
/* RISC-V kernel boot stage vm init */
#include <pgtable.h>
#include <string.h>

/********* setup memory mapping ***********/

#define PGDIR_PA (0x10200000UL + 24 * 1024 * 1024) // 4MB above heap top

uint64_t page_table = PGDIR_PA;
static uintptr_t alloc_page()
{
    // TODO:
    // a naive implementation
    // the content in PGDIR_PA is used for the top page table
    // so we alloc from the following one
    page_table += NORMAL_PAGE_SIZE;
    return page_table;
}
static void clear_mem() {
    for (uintptr_t i = page_table; i < PGDIR_PA + PAGE_SIZE * 1024; i += 8) {
        *(uint64_t *)i = 0;
    }
}
// using 2MB large page
static void map_page(uint64_t va, uint64_t pa, PTE *pgdir)
{
    // TODO:
    va >>= LARGE_PAGE_SHIFT;
    pa >>= NORMAL_PAGE_SHIFT;
    uintptr_t second_pg;
    uint64_t vpn1 = (va >> PPN_BITS) & 0x1ff, vpn2 = va & 0x1ff;
    if (((*(uint64_t *)(pgdir+vpn1)) & _PAGE_PRESENT) == 0) {
        // if this is the first time we visit the top page table
        second_pg = alloc_page() >> NORMAL_PAGE_SHIFT;
        *((uint64_t *)(pgdir+vpn1)) = (second_pg << _PAGE_PFN_SHIFT) | (!_PAGE_READ) | (!_PAGE_WRITE) | (!_PAGE_EXEC) | _PAGE_PRESENT;
        // non-leaf page table
        second_pg <<= NORMAL_PAGE_SHIFT;
    } else {
        // lookup in the top page table for allocated address
        second_pg = ((*(uint64_t *)( pgdir+vpn1) ) >> _PAGE_PFN_SHIFT) << NORMAL_PAGE_SHIFT;
    }
    *(uint64_t *)(second_pg+vpn2*(sizeof(uint64_t))) = (pa << _PAGE_PFN_SHIFT) | _PAGE_READ | _PAGE_WRITE | _PAGE_EXEC | _PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_DIRTY; // a leaf page table with rwx
    
}

/* 
 * Sv-39 mode
 */
static void setup_vm()
{
    clear_pgdir(PGDIR_PA);
    // map our boot address to virtual address
    for (int i = 0; i < 64; ++i) {
        // map 128MB to virtual address
        map_page(MAP_BASE + 0x200000 * i,MAP_BASE + 0x200000 * i,(PTE *)PGDIR_PA);
    }

    for (int i = 0; i < 8; ++i) {
        map_page(0x1f000000 + 0x200000 * i,0x1f000000 + 0x200000 * i,(PTE *)PGDIR_PA);
    }

    for (int i = 0; i < 8; ++i) {
        map_page(0x1e000000 + 0x200000 * i,0x1e000000 + 0x200000 * i,(PTE *)PGDIR_PA);
    }
    // write satp to enable paging
    set_satp(SATP_MODE_SV39, 0, PGDIR_PA >> NORMAL_PAGE_SHIFT);
    // remember to flush TLB
    local_flush_tlb_all();
    local_flush_icache_all();
}

static uintptr_t directmap(uintptr_t kva, uintptr_t pgdir)
{
    // ignore pgdir
    return kva;
}

void rt_vm_init(void)
{
    clear_mem();
    setup_vm();
}

void switch_pgdir(unsigned long pgdir) {
    set_satp(SATP_MODE_SV39, 0, pgdir >> NORMAL_PAGE_SHIFT);
    // remember to flush TLB
    local_flush_tlb_all();
    local_flush_icache_all();
}

void share_pgtable(uintptr_t dest_pgdir, uintptr_t src_pgdir)
{
    // do not realloc mem for the second level page table
    // copy the first level table only
   memcpy((void *)dest_pgdir, (void *)src_pgdir, sizeof(uint64_t) * 256);
}
#define FREEMEM (PGDIR_PA + 20 * 1024 * 1024) // 20MB above page table

/* Rounding; only works for n = power of two */
#define ROUND(a, n)     (((((uint64_t)(a))+(n)-1)) & ~((n)-1))

uintptr_t memCurr = FREEMEM;
rt_list_t freePageList;

void freePage(uintptr_t baseAddr)
{
    rt_list_insert_after(&freePageList, (rt_list_t*)(baseAddr));
}

uintptr_t realAllocPage()
{
    // cannot use generic alloc here because of alignment require
    // align PAGE_SIZE
    uintptr_t ret = ROUND(memCurr, PAGE_SIZE);
    memCurr   = ret + PAGE_SIZE;
    memset((void *)ret, 0, PAGE_SIZE);
    return ret;
}

uintptr_t allocPage()
{
    rt_base_t lock = rt_hw_interrupt_disable();
    uintptr_t ret;
    if (!rt_list_isempty(&freePageList)) {
        ret = (uintptr_t)freePageList.next;
        rt_list_remove(freePageList.next);
        memset((void *)ret, 0, PAGE_SIZE);
        rt_hw_interrupt_enable(lock);
        return ret;
    }
    ret = realAllocPage();
    rt_hw_interrupt_enable(lock);
    return ret;
}

#ifdef RT_USING_MMU
uintptr_t alloc_page_helper(uintptr_t va, uintptr_t pgdir)
{
    rt_base_t lock = rt_hw_interrupt_disable();
    // maybe should merge this function with map_one_page
    va >>= NORMAL_PAGE_SHIFT;
    uintptr_t vpn2 = (va >> PPN_BITS >> PPN_BITS) & 0x1ff,
             vpn1 = (va >> PPN_BITS) & 0x1ff,
             vpn0 = va & 0x1ff;
    uintptr_t second_pg, third_pg, ret;
    // note that we should alloc memory also for unalloced page table
    // do not forget to reuse this part of RAM after exit
    // assume the first level page table is alloced and cleaned
    if ( ((*(uint64_t *)(pgdir+vpn2*sizeof(uint64_t))) & _PAGE_PRESENT) == 0 ) {
        // the second level page table is empty
        second_pg = allocPage(); // kernel virtual address
        *(uint64_t *)(pgdir+vpn2*sizeof(uint64_t)) = (kva2pa(second_pg) >> NORMAL_PAGE_SHIFT << _PAGE_PFN_SHIFT) | (!_PAGE_READ) | (!_PAGE_WRITE) | (!_PAGE_EXEC) | (_PAGE_PRESENT);
    } else {
        second_pg = pa2kva(*(uint64_t *)(pgdir + vpn2*sizeof(uint64_t)) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
    }
    if ( ((*(uint64_t *)(second_pg+vpn1*sizeof(uint64_t))) & _PAGE_PRESENT) == 0) {
        third_pg = allocPage(); // kernel virtual address
        *(uint64_t *)(second_pg+vpn1*sizeof(uint64_t)) = (kva2pa(third_pg) >> NORMAL_PAGE_SHIFT << _PAGE_PFN_SHIFT) | (!_PAGE_READ) | (!_PAGE_WRITE) | (!_PAGE_EXEC) | (_PAGE_PRESENT);
    } else {
        third_pg = pa2kva(*(uint64_t *)(second_pg + vpn1*sizeof(uint64_t)) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
    }
    if ( ((*(uint64_t *)(third_pg+vpn0*sizeof(uint64_t))) & _PAGE_PRESENT) == 0) {
        // not alloced
        ret = allocPage(); // kernel virtual address
        *(uint64_t *)(third_pg +vpn0*sizeof(uint64_t)) = (kva2pa(ret) >> NORMAL_PAGE_SHIFT << _PAGE_PFN_SHIFT) | (_PAGE_READ) | (_PAGE_WRITE) | (_PAGE_EXEC) | (_PAGE_PRESENT) | _PAGE_USER | _PAGE_ACCESSED | _PAGE_DIRTY; 
    } else {
        // alloced, return directly
        ret = pa2kva((*(uint64_t *)(third_pg+vpn0*sizeof(uint64_t))) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
    }
    rt_hw_interrupt_enable(lock);
    return ret;
}


static void free_user_pages(uintptr_t pgdir)
{
    rt_ubase_t lock = rt_hw_interrupt_disable();
    // free all pages in pgdir and the second & third level page
    uintptr_t second_pg,third_pg,pfn;
    for (int vpn2 = 256; vpn2 < 512; ++vpn2) {
        // 256 is the lower bound of kernel ones
        if (((*(uint64_t *)(pgdir + vpn2*sizeof(uint64_t))) & _PAGE_PRESENT)) {
            // we have a valid page here
            second_pg = pa2kva(*(uint64_t *)(pgdir + vpn2*sizeof(uint64_t)) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
            for (int vpn1 = 0; vpn1 < 512; ++vpn1) {
                // full page table allows 512 offset
                if ((*(uint64_t*)(second_pg + vpn1*sizeof(uint64_t))) & _PAGE_PRESENT) {
                    third_pg = pa2kva(*(uint64_t *)(second_pg + vpn1*sizeof(uint64_t)) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
                    for (int vpn0 = 0; vpn0 < 512; ++vpn0) {
                        if (((*(uint64_t *)(third_pg+vpn0*sizeof(uint64_t))) & _PAGE_PRESENT)) {
                            pfn = pa2kva((*(uint64_t *)(third_pg+vpn0*sizeof(uint64_t))) >> _PAGE_PFN_SHIFT << NORMAL_PAGE_SHIFT);
                            freePage(pfn);
                        }
                    }
                    // need to recycle third level page table
                    freePage(third_pg);
                }
            }
            // recycle second level page now
            freePage(second_pg);
        }
    }
    rt_hw_interrupt_enable(lock);
}

void rt_cleanup_pgtable(void* pgdir) {
    // freeing up all allocated user memory
    free_user_pages((uintptr_t)pgdir);
}

void map_one_page(uintptr_t va, uintptr_t pa)
{
    uint32_t vpn2 = (va & 0x7fc0000000) >> 30;
    uint32_t vpn1 = (va & 0x3fe00000) >> 21;
    uint32_t vpn0 = (va & 0x1ff000) >> 12;
    uintptr_t next_level_pa = 0;
    PTE * pgdir = PGDIR_PA;
    if (pgdir[vpn2] == 0) {
        next_level_pa = kva2pa(allocPage());
        pgdir[vpn2] = next_level_pa >> 2;
        clear_pgdir(pa2kva(next_level_pa));
        set_attribute(pgdir + vpn2, _PAGE_PRESENT);
    }
    PTE * second_level_pgdir = pa2kva(get_pa(pgdir[vpn2]));
    if (second_level_pgdir[vpn1] == 0) {
        next_level_pa = kva2pa(allocPage());
        second_level_pgdir[vpn1] = next_level_pa >> 2;
        clear_pgdir(pa2kva(next_level_pa));
        set_attribute(second_level_pgdir + vpn1, _PAGE_PRESENT);
    }
    PTE * last_level_pgdir = pa2kva(get_pa(second_level_pgdir[vpn1]));
    last_level_pgdir[vpn0] = (pa & 0xfffffffffff000) >> 2;
    set_attribute(last_level_pgdir + vpn0,  _PAGE_PRESENT       |   _PAGE_EXEC  |
                                            _PAGE_READ          |   _PAGE_WRITE |
                                            _PAGE_ACCESSED      |   _PAGE_DIRTY);
}

#define IO_ADDR_START 0x40000000lu

// a simple allocator (doesn't design free method)
static uintptr_t io_base = IO_ADDR_START;
static inline void *alloc_io_addr(unsigned long size)
{
    uintptr_t ret = ROUND(io_base, PAGE_SIZE);
    io_base   = ret + size;
    return (void*) ret;
}

void *ioremap(unsigned long phys_addr, unsigned long size)
{
    uintptr_t va = (uintptr_t) alloc_io_addr(size);

    /* TODO: */
    for (int i = 0; i < size; i += PAGE_SIZE) {
         map_one_page(va + i, phys_addr + i);
    }

    local_flush_tlb_page(va);
    return (void*) va;
}
#endif