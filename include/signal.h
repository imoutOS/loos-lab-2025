#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"

/* ISO C99 signals.  */
#define	SIGINT		2	/* Interactive attention signal.  */
#define	SIGILL		4	/* Illegal instruction.  */
#define	SIGABRT		6	/* Abnormal termination.  */
#define	SIGFPE		8	/* Erroneous arithmetic operation.  */
#define	SIGSEGV		11	/* Invalid access to storage.  */
#define	SIGTERM		15	/* Termination request.  */

/* Historical signals specified by POSIX. */
#define	SIGHUP		1	/* Hangup.  */
#define	SIGQUIT		3	/* Quit.  */
#define	SIGTRAP		5	/* Trace/breakpoint trap.  */
#define	SIGKILL		9	/* Killed.  */
#define	SIGPIPE		13	/* Broken pipe.  */
#define	SIGALRM		14	/* Alarm clock.  */

/* Archaic names for compatibility.  */
#define	SIGIO		SIGPOLL	/* I/O now possible (4.2 BSD).  */
#define	SIGIOT		SIGABRT	/* IOT instruction, abort() on a PDP-11.  */
#define	SIGCLD		SIGCHLD	/* Old System V name */

union sigval
{
    int sival_int;
    void *sival_ptr;
};


struct siginfo_t {
   int      si_signo;     /* Signal number */
   int      si_errno;     /* An errno value */
   int      si_code;      /* Signal code */
   int      si_trapno;    /* Trap number that caused
                             hardware-generated signal
                             (unused on most architectures) */
   pid_t    si_pid;       /* Sending process ID */
   uid_t    si_uid;       /* Real user ID of sending process */
   int      si_status;    /* Exit value or signal */
   clock_t  si_utime;     /* User time consumed */
   clock_t  si_stime;     /* System time consumed */
   union sigval si_value; /* Signal value */
   int      si_int;       /* POSIX.1b signal */
   void    *si_ptr;       /* POSIX.1b signal */
   int      si_overrun;   /* Timer overrun count;
                             POSIX.1b timers */
   int      si_timerid;   /* Timer ID; POSIX.1b timers */
   void    *si_addr;      /* Memory location which caused fault */
   long     si_band;      /* Band event (was int in
                             glibc 2.3.2 and earlier) */
   int      si_fd;        /* File descriptor */
   short    si_addr_lsb;  /* Least significant bit of address
                             (since Linux 2.6.32) */
   void    *si_lower;     /* Lower bound when address violation
                             occurred (since Linux 3.19) */
   void    *si_upper;     /* Upper bound when address violation
                             occurred (since Linux 3.19) */
   int      si_pkey;      /* Protection key on PTE that caused
                             fault (since Linux 4.6) */
   void    *si_call_addr; /* Address of system call instruction
                             (since Linux 3.5) */
   int      si_syscall;   /* Number of attempted system call
                             (since Linux 3.5) */
   unsigned int si_arch;  /* Architecture of attempted system call
                             (since Linux 3.5) */
};

typedef struct siginfo_t siginfo_t;

#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))

struct sigset_t {
    unsigned long int __val[_SIGSET_NWORDS];
};

typedef struct sigset_t sigset_t;

struct sigaction {
	void     (*sa_handler)(int);
	void     (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void     (*sa_restorer)(void);
};


void do_signal(uint64_t signum);  // 实现在 syscall.c 中 TODO; refactor syscall.c signal.c

#endif /* !SIGNAL_H */
