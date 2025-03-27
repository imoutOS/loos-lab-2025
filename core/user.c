#include "fs.h"
#include "disk.h"
#include "exec.h"
#include "klib.h"
#include "proc.h"

void user_init() {
    char *argv[] = {"sh", NULL};
    char *envp[] = {NULL};
    struct task_struct *p = exec("/bin/busybox", argv, envp);
    if ( !p)
        panic("exec %s fail\n", argv[0]);
    strcpy(p->cwd, "/bin");
}
