#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t* pagetable_t;

typedef int uid_t;
typedef int gid_t;
typedef int pid_t;

typedef long clock_t;

typedef long int time_t;

struct timespec {
    time_t tv_sec;
    long int tv_nsec;
};

typedef unsigned long int dev_t;
typedef unsigned long int ino_t;
typedef unsigned int mode_t;
typedef unsigned int nlink_t;
// typedef unsigned int uid_t;
// typedef unsigned int gid_t;
typedef long int off_t;
typedef long int blksize_t;
typedef long int blkcnt_t;
typedef long int time_t;

#endif /* !TYPES_H */
