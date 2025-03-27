#ifndef __STRUCT_TIME_H__
#define __STRUCT_TIME_H__

struct timeval {
#ifdef __USE_TIME_BITS64
  __time64_t tv_sec;		/* Seconds.  */
  __suseconds64_t tv_usec;	/* Microseconds.  */
#else
  __time_t tv_sec;		/* Seconds.  */
  __suseconds_t tv_usec;	/* Microseconds.  */
#endif
};
struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

/*
typedef long int time_t;
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
*/

#endif
