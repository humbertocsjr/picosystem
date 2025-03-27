#include "picofs.h"
#include "../version.h"
#include "../hosts/hosts.h"
#include "../common.h"

FILE *_img = 0;

void error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void init_block(pfs_block_t *block, uint16_t block_number)
{
    fseek(_img, block_number * 512, SEEK_SET);
    memset(block, 0, 512);
    block->block_number = block_number;
}

void load_block(pfs_block_t *block)
{
    fseek(_img, block->block_number * 512, SEEK_SET);
    fread(block, 1, 512, _img);
    fflush(_img);
}

int load_next_block(pfs_block_t *block)
{
    if(block->next == 0) return 0;
    fseek(_img, block->next * 512, SEEK_SET);
    fread(block, 1, 512, _img);
    fflush(_img);
    return 1;
}

void store_block(pfs_block_t *block)
{
    fseek(_img, block->block_number * 512, SEEK_SET);
    fwrite(block, 1, 512, _img);
    fflush(_img);
}

void usage()
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico File System Tool v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: picofs [image] [command] [arguments ...]\n");
    printf("Commands:\n");
    printf(" genfs [size kib] [name] [bootloader]\n");
    printf(" mkfs [size kib] [name] [bootloader]\n");
    exit(1);
}

uint16_t alloc_block(pfs_block_t *block, uint16_t parent)
{
    pfs_block_t prev_block;
    uint16_t size;
    init_block(block, 0);
    load_block(block);
    size = block->data;
    load_next_block(block);
    memcpy(&prev_block, block, 512);
    load_next_block(block);
    if(block->next == 0)
    {
        prev_block.next ++;
        if(prev_block.next >= size)
        {
            return 0;
        }
        store_block(&prev_block);
        init_block(&prev_block, prev_block.next);
        store_block(&prev_block);
    }
    else
    {
        prev_block.next = block->next;
        store_block(&prev_block);
    }
    init_block(block, block->block_number);
    block->parent = parent;
    store_block(block);
    return block->block_number;
}

void genfs(int argc, char **argv)
{
    if(argc < 3 || argc > 4 ) usage();
    uint16_t size = atoi(argv[1]);
    pfs_block_t block;
    init_block(&block, size * 2 - 1);
    store_block(&block);
    init_block(&block, 0);
    load_block(&block);
    if(argc == 4)
    {
        FILE *bootfile = fopen(argv[3], "rb");
        if(!bootfile) error("can't open file: %s", argv[3]);
        fread(&block, 1, 500, bootfile);
        fclose(bootfile);
    }
    block.configs = 0xaa55;
    block.data = size;
    block.next = 1;
    block.parent = 0;
    block.resources = PFS_SIGNATURE;
    store_block(&block);
    init_block(&block, 1);
    copy(block.inode.name, argv[2], PFS_NAME_SIZE);
    block.inode.mode = PFS_MODE_DIR | PFS_MODE_USRR | PFS_MODE_USRW | PFS_MODE_GRPR | PFS_MODE_GRPW | PFS_MODE_OTHR | PFS_MODE_OTHW;
    block.next = 2;
    store_block(&block);
    init_block(&block, 2);
    store_block(&block);
}


void add(int argc, char **argv)
{
    
}


int main(int argc, char **argv)
{
    if(argc < 3) usage();
    if(streq(argv[2], "genfs"))
        _img = fopen(argv[1], "wb+");
    else
        _img = fopen(argv[1], "rb+");
    if(!_img) error("can't open image file: %s", argv[1]);
    argc--;
    argc--;
    argv++;
    argv++;
    if(streq(argv[0], "genfs"))
    {
        genfs(argc, argv);
    }
    else if(streq(argv[0], "mkfs"))
    {
        genfs(argc, argv);
    }
    else error("command unknown: %s", argv[0]);
    fclose(_img);
}