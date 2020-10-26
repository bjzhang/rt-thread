/*
Copyright 2019 Chen Guokai

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef INCLUDE_XIMOFS_H_
#define INCLUDE_XIMOFS_H_

#include <sbi.h>
#include <pgtable.h>
#include <os/stdio.h>
#include <os/string.h>

#define MAX_FILE_SECTOR 128
/*
    XIMO Filesystem
    Design detail:
        superblock: 1 block, 
            4 byte ASCII 'XIMO'
            4 byte uint32_t filesystem total size in blocks
            4 byte uint32_t filesystem sector map offset in blocks for BBL (should always be 1 for now)
            4 byte uint32_t filesystem sector map size (For 1G size should be at max 1,073,741,824 / 512 / 32 = 0x40000 byte)
            4 byte uint32_t filesystem inode map offset for BBL
            4 byte uint32_t filesystem inode map size in blocks
            4 byte uint32_t filesystem inode pool size in blocks
            4 byte uint32_t filesystem data pool offset for BBL
            4 byte uint32_t filesystem data pool size in blocks
            4 byte uint32_t filesystem sector map used
            4 byte uint32_t filesystem inode map used
            4 byte uint32_t filesystem root directory inode offset in blocks (every inode is aligned)
        sector map: for now 0x20000 byte, 0x100 blocks, max 1GB
            every 0 bit indicates an empty block
        inode map: for now 0x400 byte, 2 blocks
            every 0 bit indicates an empty inode
        inode pool: inode_size * 0x2000
            inode_size = 4096 byte = 8 blocks
            only support two level inode 
            every first level inode contains the essential info for one file/dir:
                file size: 4 byte
                file type: 4 byte
                file permission: 4 byte
                file name: 128 byte
                second level inode offsets in blocks:
                    every offset uses 4 byte
                    in total 989 offsets
            every second level inode contains only the offset for blocks
                every offset uses 4 byte
                in total 1024 offsets
            offset 0xffffffff is NULL
        

        Due to the limitation of available memory for ximoFS,
        we only maintain the superblock, sector map, inode map in RAM
        inode pool and data will always be on the disk

        A dir should always use only 8 sector, thus support max 31 files / dirs in one dir

*/
#define SECTOR_SIZE 512

#define SUPERBLOCK_SIZE_S 1

/* sector map */
#define SECTOR_MAP_SIZE 0x20000
#define SECTOR_MAP_SIZE_S (SECTOR_MAP_SIZE / SECTOR_SIZE)

/* every single inode */
#define INODE_SIZE 0x1000
#define INODE_SIZE_S (INODE_SIZE / SECTOR_SIZE)

/* inode map */
#define INODE_MAP_SIZE 0x400
#define INODE_MAP_SIZE_S (INODE_MAP_SIZE / SECTOR_SIZE)

/* inode pool */
#define INODE_POOL_SIZE (INODE_SIZE * 0x2000)
#define INODE_POOL_SIZE_S (INODE_POOL_SIZE / SECTOR_SIZE)

#define NULL_OFFSET 0xffffffff

#define DATA_SIZE_S 0x200000
#define DATA_SIZE (DATA_SIZE_S * SECTOR_SIZE)

/* ximoFS begins from  */
#define FS_OFFSET (512 * 1024 * 1024)
#define FS_OFFSET_S (FS_OFFSET / SECTOR_SIZE)

#define MAGLEN 4
#define BITLEN 8


struct superblock_t {
    uint32_t MAG;
    uint32_t fs_total_size;
    uint32_t sector_map_off;
    uint32_t sector_map_size;
    uint32_t inode_map_off;
    uint32_t inode_map_size;
    uint32_t inode_pool_off;
    uint32_t inode_pool_size;
    uint32_t data_pool_off;
    uint32_t data_pool_size;
    
    // not used for now
    uint32_t sector_map_used;
    // data pool used is the same as sector map used
    uint32_t inode_map_used;
    // inode pool used is the same as inode map used
    uint32_t root_dir_off;
    uint32_t uid;
};
extern struct superblock_t superblock;
#define NAME_LEN 128
#define FIRST_INODE_OFFSET_CNT 1020
#define SECOND_INODE_OFFSET_CNT 1023

#define FILE_TYPE_DIR 0x0
#define FILE_TYPE_FILE 0x1
/* unused for now */
#define FILE_TYPE_LINK 0x2

#define INVALIDPATH 0xffffffff
#define BUFSIZE 128

struct first_inode_t {
    uint32_t size;
    uint32_t type;
    uint32_t permission;
    uint32_t uid;
    uint32_t second_offset[FIRST_INODE_OFFSET_CNT];
};

struct second_inode_t {
    uint32_t uid;
    uint32_t sector_offset[SECOND_INODE_OFFSET_CNT];
};


#define DENTRY_CNT 31

/* 128B per entry */
struct dentry_t {
    uint32_t type;
    uint32_t offset;
    char name[NAME_LEN-sizeof(uint32_t)*2];
};

struct dir_data_t {
    uint32_t total;
    uint32_t foo[31]; // align dentry and reserve some space
    struct dentry_t dentry[DENTRY_CNT];
};

void cd(char* path);
void ls();
void rmdir(char *dirname);
uint32_t mkdir(char * path);
void mkfs();
void statfs();
void initfd();

/*
 * A file descriptioner should contain the following information:
 * valid: 1 is valid, 0 is invalid
 * path: denoted with the first level inode offset
 * permission: R & W
 * current position: in bytes, default to 0
*/

/*
 * when opening a file, allocate a free file descriptioner
 * when closing a file, the file descriptioner will be recycled
*/

struct fd_t {
    uint32_t valid;
    uint32_t first_inode_off;
    uint32_t permission;
    uint32_t curr_r_pos; // according to the test program, read
    uint32_t curr_w_pos; // and write have seperate pointers OAO
};
#define FDSIZE 32
struct fd_t global_fd[FDSIZE];
void touch(char *name);
void cat(char *name);
void initfd();
int open(char*name, int access);
int read(int fd, char *buff, int size);
int write(int fd, char *buff, int size);
void close(int fd);
void destroy();
#endif