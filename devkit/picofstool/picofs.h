#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#pragma pack(1)

#define streq(s1, s2) !strcmp(s1, s2)
#define strneq(s1, s2, n) !strmcmp(s1, s2, n)

#define PFS_SIGNATURE 0x8919
#define PFS_NAME_SIZE 64

enum
{
    PFS_MODE_USRR   = 00400,
    PFS_MODE_USRW   = 00200,
    PFS_MODE_USRX   = 00100,
    PFS_MODE_GRPR   = 00040,
    PFS_MODE_GRPW   = 00020,
    PFS_MODE_GRPX   = 00010,
    PFS_MODE_OTHR   = 00004,
    PFS_MODE_OTHW   = 00002,
    PFS_MODE_OTHX   = 00001,
    PFS_MODE_FILE   = 01000,
    PFS_MODE_DIR    = 02000
};

#define pfs_datetime_t struct pfs_datetime
struct pfs_datetime
{
    int8_t year; // year+2000
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

#define pfs_inode_t struct pfs_inode
struct pfs_inode
{
    char name[64];
    uint16_t mode;
    uint16_t user_id;
    uint16_t group_id;
    pfs_datetime_t creation;
    pfs_datetime_t modification;
    uint8_t reserved[418];
};

#define pfs_block_t struct pfs_block
struct pfs_block
{
    union
    {
        uint8_t raw[500];
        pfs_inode_t inode;
    };
    uint16_t block_number;
    uint16_t next;
    uint16_t parent;
    uint16_t data;
    uint16_t resources;
    uint16_t configs;
};

#pragma pack()