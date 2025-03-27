/*
 * ext2.c
 * Copyright (C) 2025 Tiger1218 <tiger1218@foxmail.com>
 *
 * Distributed under terms of the GNU AGPLv3 license.
 */

#include "ext2.h"
#include "ahci.h"
#include "mm.h"
#include "errno.h"

#define BLOCK_SIZE 1024
#define _min(a,b) ( ((a)<(b)) ? (a) : (b))
#define _max(a,b) ( ((a)>(b)) ? (a) : (b))

struct ext2_fs e2fs;

int gport;
HBA_MEM* gabar;

int read_block(uint32_t block_offset, char* buf) {
    assert(BLOCK_SIZE % 512 == 0);
    ahci_read(gabar, gport, block_offset * ( BLOCK_SIZE / 512 ), BLOCK_SIZE / 512, (uint8_t*)buf);
    return 0;
}

// 读取超级块
int ext2_read_superblock(struct ext2_fs *fs, uint32_t block_offset) {
    // 假设 block_size 为1024（需要根据实际检测）
    char* buf = kmalloc(BLOCK_SIZE);
    read_block(block_offset, buf);
    memcpy(&fs->sb, buf, sizeof(struct ext2_superblock));
    kfree(buf);
    // read_block(block_offset, (char*)&fs->sb);
    
    if (fs->sb.s_magic != EXT2_SIGNATURE) {
        return -1;  // 无效的ext2文件系统
    }
    
    // 计算块大小（实际需要根据sb.s_log_block_size计算）
    fs->block_size = 1024 << fs->sb.s_log_block_size;
    return 0;
}

// 读取块组描述符
void ext2_read_group_desc(struct ext2_fs *fs) {
    // 块组描述符起始块：对于块大小=1024，从块2开始
    uint32_t bgd_block = (fs->block_size == 1024) ? 2 : 1;
    char* buf = kmalloc(BLOCK_SIZE);
    read_block(bgd_block, buf);
    memcpy(&fs->gd, buf, sizeof(struct ext2_group_desc));
    kfree(buf);
}

// 获取指定inode
int ext2_get_inode(struct ext2_fs *fs, uint32_t inode_num, struct ext2_inode *inode) {
    // 验证inode号有效性
    if (inode_num == 0 || inode_num > fs->sb.s_inodes_count) {
        return -1; // 无效的inode号
    }

    // 计算块组号和组内索引
    uint32_t group = (inode_num - 1) / fs->sb.s_inodes_per_group;
    uint32_t index = (inode_num - 1) % fs->sb.s_inodes_per_group;

    // 计算块组描述符位置
    uint32_t bgdt_start_block = fs->sb.s_first_data_block + 1; // 描述符表起始块
    uint32_t desc_per_block = fs->block_size / sizeof(struct ext2_group_desc);
    uint32_t desc_block = bgdt_start_block + (group / desc_per_block);
    uint32_t desc_offset = (group % desc_per_block) * sizeof(struct ext2_group_desc);

    // 读取块组描述符
    char *desc_buf = kmalloc(fs->block_size);
    if (!desc_buf || read_block(desc_block, desc_buf) != 0) {
        kfree(desc_buf);
        return -2; // 读取失败
    }
    struct ext2_group_desc *gd = (struct ext2_group_desc*)(desc_buf + desc_offset);
    uint32_t inode_table_block = gd->bg_inode_table;
    kfree(desc_buf);

    // 计算inode表位置
    uint32_t inode_size = fs->sb.s_inode_size;
    uint32_t inode_offset = index * inode_size;
    uint32_t block_offset = inode_table_block + (inode_offset / fs->block_size);
    uint32_t intra_block_offset = inode_offset % fs->block_size;

    // 验证偏移是否越界
    if (intra_block_offset + sizeof(struct ext2_inode) > fs->block_size) {
        return -3; // 跨块inode（需要特殊处理）
    }

    // 读取数据块
    char *block_buf = kmalloc(fs->block_size);
    if (!block_buf || read_block(block_offset, block_buf) != 0) {
        kfree(block_buf);
        return -4; // 块读取失败
    }

    // 复制inode数据
    memcpy(inode, block_buf + intra_block_offset, sizeof(struct ext2_inode));
    kfree(block_buf);

    // 检查inode有效性
    if (inode->i_mode == 0) {
        return -5; // 未使用的inode
    }

    return 0;
}

// 读取目录条目
void ext2_read_dir(struct ext2_fs *fs, struct ext2_inode *dir_inode) {
    char * block = kmalloc(fs->block_size);
    for (int i = 0; i < 12; i++) {  // 只处理直接块
        if (dir_inode->i_block[i] == 0) break;
        
        read_block(dir_inode->i_block[i], block);
        
        struct ext2_dir_entry *entry = (struct ext2_dir_entry*)block;
        while ((char*)entry < block + fs->block_size) {
            // 处理目录条目
            char name[entry->name_len + 1];
            memcpy(name, entry->name, entry->name_len);
            name[entry->name_len] = '\0';
            
            // 输出文件名和inode号
            Log("Found: %s (inode %d)\n", name, entry->inode);
            
            if (entry->rec_len == 0) break;
            entry = (struct ext2_dir_entry*)((char*)entry + entry->rec_len);
        }
    }
}

// 初始化ext2文件系统
int ext2_init(struct ext2_fs *fs, uint32_t partition_start) {
    // 读取超级块（位于分区的第一个块，偏移1024字节处）
    if (ext2_read_superblock(fs, partition_start + 1) != 0) {
        return -1;
    }
    
    // 读取块组描述符
    ext2_read_group_desc(fs);
    
    // 获取根目录inode（固定为2号inode）
    struct ext2_inode root_inode;
    ext2_get_inode(fs, EXT2_ROOT_INO, &root_inode);
    
    // 读取根目录内容
    ext2_read_dir(fs, &root_inode);
    return 0;
}

void ext2_init_disk(HBA_MEM *abar, int port_num) {
    gabar = abar;
    gport = port_num;
    ext2_init(&e2fs, 0);
}


bool ext2_check(uint8_t *buf) {
    struct ext2_superblock *sb = (void*)(buf + 1024);
    return sb->s_magic == EXT2_SIGNATURE;
}

void ext2_test() {
    struct ext2_fs fs;
    ext2_init(&fs, 0);
    halt(0);
}

#define EXT2_ROOT_INO 2
#define EXT2_S_IFDIR 0x4000
#define EXT2_FT_DIR 2
#define EXT2_NAME_LEN 255

// 路径解析状态机
static int parse_path_component(const char **path, char *name) {
    const char *p = *path;
    int len = 0;

    // 跳过前置斜杠
    while (*p == '/') p++;

    // 提取组件名称
    while (*p != '\0' && *p != '/' && len < EXT2_NAME_LEN) {
        name[len++] = *p++;
    }
    name[len] = '\0';

    // 更新路径指针
    *path = p;
    return len;
}

// 目录条目搜索
static int find_dir_entry(struct ext2_fs *fs, struct ext2_inode *dir,
                         const char *name, uint32_t *ino) {
    char block_buf[fs->block_size];
    // uint32_t block_idx = 0;

    // 遍历所有直接块
    for (int i = 0; i < 12; i++) { // 直接块
        if (dir->i_block[i] == 0) break;

        read_block(dir->i_block[i], block_buf);
        struct ext2_dir_entry *de = (struct ext2_dir_entry *)block_buf;
        char *limit = block_buf + fs->block_size;

        while ((char *)de < limit) {
            // 跳过无效条目
            if (de->inode == 0 || de->name_len == 0) {
                de = (struct ext2_dir_entry*)((char*)de + de->rec_len);
                continue;
            }

            // 名称比较
            char de_name[EXT2_NAME_LEN + 1];
            memcpy(de_name, de->name, de->name_len);
            de_name[de->name_len] = '\0';

            if (strcmp(de_name, name) == 0) {
                *ino = de->inode;
                return 0;
            }

            // 跳转到下一个条目
            de = (struct ext2_dir_entry*)((char*)de + de->rec_len);
        }
    }

    // TODO: 处理间接块
    return -1;
}

// 主路径查找函数
int ext2_path_lookup(struct ext2_fs *fs, const char *path, struct ext2_inode *result) {
    char component[EXT2_NAME_LEN + 1];
    struct ext2_inode current_inode;
    int ret;

    // 获取根目录inode
    if ((ret = ext2_get_inode(fs, EXT2_ROOT_INO, &current_inode)) != 0) {
        return ret;
    }

    // 空路径直接返回根目录
    if (path[0] == '\0') {
        memcpy(result, &current_inode, sizeof(struct ext2_inode));
        return 0;
    }

    const char *p = path;
    while (*p != '\0') {
        // 解析路径组件
        int len = parse_path_component(&p, component);
        if (len == 0) continue;

        // 目录有效性检查
        if (!(current_inode.i_mode & EXT2_S_IFDIR)) {
            return -ENOTDIR;
        }

        // 查找目录条目
        uint32_t next_ino;
        if ((ret = find_dir_entry(fs, &current_inode, component, &next_ino)) != 0) {
            return ret;
        }

        // 获取下一级inode
        if ((ret = ext2_get_inode(fs, next_ino, &current_inode)) != 0) {
            return ret;
        }
    }

    memcpy(result, &current_inode, sizeof(struct ext2_inode));
    return 0;
}

uint8_t ext2_type_to_dtype(uint8_t ext2_type) {
    switch (ext2_type) {
        case 0x0: return 0; /* DT_UNK */
        case 0x1: return 8; /* DT_REG */
        case 0x2: return 4; /* DT_DIR */
        case 0x3: return 2; /* DT_CHR */
        case 0x4: return 6; /* DT_BLK */
        case 0x5: return 1; /* DT_FIFO */
        case 0x6: return 12; /* DT_SOCK */
        case 0x7: return 10; /* DT_LNK */
        default: return 0; /* DT_UNK */
    }
}

static uint32_t get_physical_block(struct ext2_inode *inode, uint32_t logical_block) {
    uint32_t ptrs_per_block = e2fs.block_size / sizeof(uint32_t);
    uint32_t direct_blocks = 12;

    if (logical_block < direct_blocks) {
        return inode->i_block[logical_block];
    }

    logical_block -= direct_blocks;
    if (logical_block < ptrs_per_block) { // 一级间接块
        uint32_t indirect_block = inode->i_block[12];
        uint32_t buffer[ptrs_per_block];
        read_block(indirect_block, (char*)buffer);
        return buffer[logical_block];
    }

    logical_block -= ptrs_per_block;
    if (logical_block < ptrs_per_block*ptrs_per_block) { // 二级间接块
        uint32_t double_indirect_block = inode->i_block[13];
        uint32_t idx1 = logical_block / ptrs_per_block;
        uint32_t idx2 = logical_block % ptrs_per_block;

        uint32_t first_level[ptrs_per_block];
        read_block(double_indirect_block, (char*)first_level);

        uint32_t second_level[ptrs_per_block];
        read_block(first_level[idx1], (char*)second_level);

        return second_level[idx2];
    }

    logical_block -= ptrs_per_block*ptrs_per_block; // 三级间接块
    uint32_t triple_indirect_block = inode->i_block[14];
    uint32_t idx1 = logical_block / (ptrs_per_block*ptrs_per_block);
    uint32_t idx2 = (logical_block / ptrs_per_block) % ptrs_per_block;
    uint32_t idx3 = logical_block % ptrs_per_block;

    uint32_t first_level[ptrs_per_block];
    read_block(triple_indirect_block, (char*)first_level);

    uint32_t second_level[ptrs_per_block];
    read_block(first_level[idx1], (char*)second_level);

    uint32_t third_level[ptrs_per_block];
    read_block(second_level[idx2], (char*)third_level);

    return third_level[idx3];
}

void ext2_fread(struct ext2_inode *inode, char* buf, uint32_t count, uint32_t *size) {
    uint32_t file_size = inode->i_size;
    uint32_t block_size = e2fs.block_size;
    uint32_t bytes_read = 0;
    uint32_t start_offset = 0; // 假设从文件起始位置读取

    // 计算实际可读字节数
    uint32_t max_read = (start_offset > file_size) ? 0 : file_size - start_offset;
    count = _min(count, max_read);

    // 按块进行读取
    while (bytes_read < count) {
        uint32_t current_block = (start_offset + bytes_read) / block_size;
        uint32_t block_offset = (start_offset + bytes_read) % block_size;
        uint32_t bytes_in_block = block_size - block_offset;
        uint32_t bytes_needed = count - bytes_read;
        uint32_t bytes_to_copy = _min(bytes_in_block, bytes_needed);

        // 获取物理块号
        uint32_t phys_block = get_physical_block(inode, current_block);
        if (phys_block == 0) {
            // 稀疏文件处理：填充零
            memset(buf + bytes_read, 0, bytes_to_copy);
            bytes_read += bytes_to_copy;
            continue;
        }

        // 读取块数据
        char block_data[block_size];
        read_block(phys_block, block_data);

        // 内存拷贝
        memcpy(buf + bytes_read, block_data + block_offset, bytes_to_copy);
        bytes_read += bytes_to_copy;
    }

    *size = bytes_read;
}
