/*
 * ext2.h
 * Copyright (C) 2025 Tiger1218 <tiger1218@foxmail.com>
 *
 * Distributed under terms of the GNU AGPLv3 license.
 */

#ifndef EXT2_H
#define EXT2_H

#include "klib.h"
#include "ahci.h"

#define EXT2_SIGNATURE 0xEF53
#define EXT2_ROOT_INO 2
#define EXT2_S_IFREG 0x8000  // 常规文件类型
#define EXT2_S_IFDIR 0x4000  // 目录类型
                             //

// 超级块结构 (Table 3.3)
struct ext2_superblock {
    uint32_t s_inodes_count;        // Inode总数
    uint32_t s_blocks_count;        // 块总数
    uint32_t s_r_blocks_count;      // 保留块数
    uint32_t s_free_blocks_count;   // 空闲块数
    uint32_t s_free_inodes_count;   // 空闲Inode数
    uint32_t s_first_data_block;    // 首个数据块号
    uint32_t s_log_block_size;      // 块大小计算参数
    uint32_t s_log_frag_size;       // 片段大小计算参数
    uint32_t s_blocks_per_group;    // 每组块数
    uint32_t s_frags_per_group;     // 每组片段数
    uint32_t s_inodes_per_group;    // 每组Inode数
    uint32_t s_mtime;               // 最后挂载时间
    uint32_t s_wtime;               // 最后写入时间
    uint16_t s_mnt_count;           // 挂载次数
    uint16_t s_max_mnt_count;       // 最大挂载次数
    uint16_t s_magic;               // 魔数0xEF53
    uint16_t s_state;               // 文件系统状态
    uint16_t s_errors;              // 错误处理方式
    uint16_t s_minor_rev_level;     // 次版本号
    uint32_t s_lastcheck;           // 最后检查时间
    uint32_t s_checkinterval;       // 检查间隔
    uint32_t s_creator_os;          // 创建操作系统
    uint32_t s_rev_level;           // 主版本号
    uint16_t s_def_resultd;         // 保留块默认UID
    uint16_t s_def_resgid;          // 保留块默认GID
    
    // EXT2_DYNAMIC_REV特定字段
    uint32_t s_first_ino;           // 首个可用Inode号
    uint16_t s_inode_size;          // Inode结构大小
    uint16_t s_block_group_nr;      // 块组号
    uint32_t s_feature_compat;      // 兼容特性
    uint32_t s_feature_incompat;    // 不兼容特性
    uint32_t s_feature_ro_compat;   // 只读兼容特性
    uint8_t  s_uuid[16];            // 卷UUID
    char     s_volume_name[16];     // 卷名称
    char     s_last_mounted[64];    // 最后挂载路径
    uint32_t s_algo_bitmap;         // 压缩算法位图
    
    // 性能提示
    uint8_t  s_prealloc_blocks;     // 预分配块数
    uint8_t  s_prealloc_dir_blocks; // 目录预分配块数
    uint16_t s_padding1;            // 对齐填充
    
    // 日志支持
    uint8_t  s_journal_uuid[16];    // 日志UUID
    uint32_t s_journal_inum;        // 日志Inode号
    uint32_t s_journal_dev;         // 日志设备号
    uint32_t s_last_orphan;         // 待删除Inode链表
    
    // 目录索引
    uint32_t s_hash_seed[4];        // 哈希种子
    uint8_t  s_def_hash_version;    // 默认哈希版本
    uint8_t  s_padding2[3];         // 填充
    
    // 其他选项
    uint32_t s_default_mount_options;
    uint32_t s_first_meta_bg;       // 首个元块组
    uint32_t s_reserved[190];       // 保留空间
} __attribute__((packed));

// 块组描述符 (Table 3.12)
struct ext2_group_desc {
    uint32_t bg_block_bitmap;       // 块位图块号
    uint32_t bg_inode_bitmap;       // Inode位图块号
    uint32_t bg_inode_table;        // Inode表起始块号
    uint16_t bg_free_blocks_count;  // 本组空闲块数
    uint16_t bg_free_inodes_count;  // 本组空闲Inode数
    uint16_t bg_used_dirs_count;    // 目录Inode数
    uint16_t bg_pad;                // 填充
    uint8_t  bg_reserved[12];       // 保留
} __attribute__((packed));

// Inode结构 (Table 3.13)
struct ext2_inode {
    uint16_t i_mode;        // 文件类型和权限
    uint16_t i_uid;         // 用户ID低16位
    uint32_t i_size;         // 文件大小（低32位）
    uint32_t i_atime;        // 最后访问时间
    uint32_t i_ctime;        // 创建时间
    uint32_t i_mtime;        // 最后修改时间
    uint32_t i_dtime;        // 删除时间
    uint16_t i_gid;         // 组ID低16位
    uint16_t i_links_count; // 硬链接计数
    uint32_t i_blocks;       // 512字节块计数
    uint32_t i_flags;        // 文件标志位
    uint32_t i_osd1;         // OS依赖字段1
    uint32_t i_block[15];    // 数据块指针
    uint32_t i_generation;   // 文件版本（NFS）
    uint32_t i_file_acl;     // 扩展属性块
    uint32_t i_dir_acl;      // 目录ACL或大小高32位
    uint32_t i_faddr;        // 片段地址
    union {
        struct {            // HURD特定
            uint8_t  h_i_frag;
            uint8_t  h_i_fsize;
            uint16_t h_i_mode_high;
            uint16_t h_i_uid_high;
            uint16_t h_i_gid_high;
            uint32_t h_i_author;
        };
        struct {            // Linux特定
            uint8_t  l_i_frag;
            uint8_t  l_i_fsize;
            uint16_t l_pad;
            uint16_t l_i_uid_high;
            uint16_t l_i_gid_high;
            uint32_t l_i_reserved;
        };
    } i_osd2;
} __attribute__((packed));

// 目录条目结构 (Table 4.1)
struct ext2_dir_entry {
    uint32_t inode;          // Inode号
    uint16_t rec_len;        // 条目总长度
    uint8_t  name_len;       // 文件名长度
    uint8_t  file_type;      // 文件类型
    char     name[];         // 文件名（变长）
} __attribute__((packed));

// 扩展属性头 (Table 5.2)
struct ext2_xattr_header {
    uint32_t h_magic;       // 魔数0xEA020000
    uint32_t h_refcount;    // 引用计数
    uint32_t h_blocks;      // 使用块数
    uint32_t h_hash;        // 哈希值
    uint8_t  reserved[16];  // 保留
} __attribute__((packed));

// 索引目录根结构 (Table 4.4)
struct ext2_dx_root {
    struct ext2_dir_entry dot;      // "."条目
    struct ext2_dir_entry dotdot;   // ".."条目
    uint32_t reserved_zero;         // 保留
    uint8_t  hash_version;          // 哈希版本
    uint8_t  info_length;           // 信息长度
    uint8_t  indirect_levels;       // 间接层级
    uint8_t  unused_flags;          // 未使用标志
} __attribute__((packed));

struct ext2_fs {
    struct ext2_superblock sb;
    struct ext2_group_desc gd;
    uint32_t block_size;
    uint32_t inode_size;
    uint32_t blocks_per_group;
};


// Function Definations

bool ext2_check(uint8_t *buf);
void ext2_init_disk(HBA_MEM *abar, int port_num) ;
void ext2_test();
int read_block(uint32_t block_offset, char* buf) ;
int ext2_path_lookup(struct ext2_fs *fs, const char *path, struct ext2_inode *result);
void ext2_fread(struct ext2_inode * inode, char* buf, uint32_t count, uint32_t *size);
uint8_t ext2_type_to_dtype(uint8_t ext2_type) ;

extern struct ext2_fs e2fs;

#define EXT2_NDIR_BLOCKS 12


#endif /* !EXT2_H */
