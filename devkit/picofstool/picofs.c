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
    //printf("[R%d NEXT%d DATA%d]\n", block->block_number, block->next, block->data);
}

int load_next_block(pfs_block_t *block)
{
    if(block->next == 0) return 0;
    fseek(_img, block->next * 512, SEEK_SET);
    fread(block, 1, 512, _img);
    fflush(_img);
    //printf("[R%d NEXT%d DATA%d]\n", block->block_number, block->next, block->data);
    return 1;
}

void store_block(pfs_block_t *block)
{
    //printf("[W%d NEXT%d DATA%d]\n", block->block_number, block->next, block->data);
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
    printf(" add [local file] [destination address]\n");
    printf(" add_file [local file] [destination address]\n");
    printf(" add_app [local file] [destination address]\n");
    printf(" mkdir [address]\n");
    printf(" df\n");
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
    block.data = size << 1; // Current usable size (differs from partition size if shinked for swap file use)
    block.parent = size << 1; // Current partition size
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


void add(int argc, char **argv, uint16_t mode_extra)
{
    if(argc != 3) usage();
    pfs_block_t dir;
    pfs_block_t item;
    pfs_block_t data;
    int size;
    char name[PFS_NAME_SIZE];
    char c[2];
    char *ptr = argv[2];
    int found;
    FILE *file = fopen(argv[1], "rb");
    if(!file) error("can't open file: %s", argv[1]);
    c[1] = 0;
    copy(name, "", PFS_NAME_SIZE);
    init_block(&dir, 0);
    load_block(&dir);
    load_next_block(&dir);
    ptr--;
    do
    {
        ptr++;
        if(*ptr == '/' || *ptr == 0)
        {
            if(strlen(name) == 0 && *ptr == 0) error("invalid file address.");
            if(strlen(name) != 0)
            {
                found = 0;
                if(dir.data != 0)
                {
                    init_block(&item, dir.data);
                    load_block(&item);
                    do
                    {
                        if(strneq(item.inode.name, name, PFS_NAME_SIZE))
                        {
                            memcpy(&dir, &item, sizeof(pfs_block_t));
                            found = 1;
                            break;
                        }
                        if(!item.next) break;
                        load_next_block(&item);
                    } while(1);
                }
                if(!found)
                {
                    if(!alloc_block(&item, dir.block_number)) error("disk is full.");
                    load_block(&dir);
                    item.next = dir.data;
                    dir.data = item.block_number;
                    if((dir.inode.mode & PFS_MODE_DIR) == 0) error("%s is not a directory.", dir.inode.name);
                    copy(item.inode.name, name, PFS_NAME_SIZE);
                    item.inode.mode = (*ptr == '/' ? PFS_MODE_DIR : (PFS_MODE_FILE | mode_extra))  | PFS_MODE_USRR | PFS_MODE_USRW | PFS_MODE_GRPR | PFS_MODE_GRPW | PFS_MODE_OTHR | PFS_MODE_OTHW;
                    store_block(&item);
                    store_block(&dir);
                    memcpy(&dir, &item, sizeof(pfs_block_t));
                }
                copy(name, "", PFS_NAME_SIZE);
            }
            if(*ptr == 0) break;
        }
        else
        {
            c[0] = *ptr;
            concat(name, c, PFS_NAME_SIZE);
        }
    } while(*ptr);
    if(!alloc_block(&item, dir.block_number)) error("disk is full");
    load_block(&dir);
    dir.data = item.block_number;
    store_block(&dir);
    do
    {
        memcpy(&data, &item, 512);
        size = fread(data.raw, 1, 500, file);
        if(size == 500)
        {
            if(!alloc_block(&item, dir.block_number)) error("disk is full");
            data.next = item.block_number;
        }
        store_block(&data);
    } while(size == 500);

}

void makedir(int argc, char **argv)
{
    if(argc != 2) usage();
    pfs_block_t dir;
    pfs_block_t item;
    char name[PFS_NAME_SIZE];
    char c[2];
    char *ptr = argv[1];
    int found;
    c[1] = 0;
    copy(name, "", PFS_NAME_SIZE);
    init_block(&dir, 0);
    load_block(&dir);
    load_next_block(&dir);
    ptr--;
    do
    {
        ptr++;
        if(*ptr == '/' || *ptr == 0)
        {
            if(strlen(name) != 0)
            {
                found = 0;
                if(dir.data != 0)
                {
                    init_block(&item, dir.data);
                    load_block(&item);
                    do
                    {
                        if(strneq(item.inode.name, name, PFS_NAME_SIZE))
                        {
                            memcpy(&dir, &item, sizeof(pfs_block_t));
                            found = 1;
                            break;
                        }
                        if(!item.next) break;
                        load_next_block(&item);
                    } while(1);
                }
                if(!found)
                {
                    if(!alloc_block(&item, dir.block_number)) error("disk is full.");
                    load_block(&dir);
                    item.next = dir.data;
                    dir.data = item.block_number;
                    if((dir.inode.mode & PFS_MODE_DIR) == 0) error("%s is not a directory.", dir.inode.name);
                    copy(item.inode.name, name, PFS_NAME_SIZE);
                    item.inode.mode = PFS_MODE_DIR | PFS_MODE_USRR | PFS_MODE_USRW | PFS_MODE_GRPR | PFS_MODE_GRPW | PFS_MODE_OTHR | PFS_MODE_OTHW;
                    store_block(&item);
                    store_block(&dir);
                    memcpy(&dir, &item, sizeof(pfs_block_t));
                }
                copy(name, "", PFS_NAME_SIZE);
            }
            if(*ptr == 0) break;
        }
        else
        {
            c[0] = *ptr;
            concat(name, c, PFS_NAME_SIZE);
        }
    } while(*ptr);
}

void df(int argc, char **argv)
{
    pfs_block_t block;
    uint16_t size;
    if(argc != 1) usage();
    init_block(&block, 0);
    load_block(&block);
    size = block.data;
    while(block.next)
    {
        load_next_block(&block);
    }
    printf("FS\tBlocks\tIn Use\tFree\tUsed%%\n");
    printf("/\t%6d\t%6d\t%4d\t%d%%\n", size >> 1, block.block_number >> 1, (size - block.block_number) >> 1,  ((unsigned)(block.block_number) * 100) / ((unsigned)size * 100));
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
    else if(streq(argv[0], "add") || streq(argv[0], "add_file"))
    {
        add(argc, argv, 0);
    }
    else if(streq(argv[0], "add_app"))
    {
        add(argc, argv, PFS_MODE_GRPX | PFS_MODE_OTHX | PFS_MODE_USRX);
    }
    else if(streq(argv[0], "df"))
    {
        df(argc, argv);
    }
    else if(streq(argv[0], "mkdir"))
    {
        makedir(argc, argv);
    }
    else error("command unknown: %s", argv[0]);
    fclose(_img);
}