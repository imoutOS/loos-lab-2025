// exec.c
struct task_struct *exec(char *path, char *argv[], char *envp[]);
int exec_file(char *path);
void user_init();
struct task_struct *fork(struct task_struct *p) ;
struct task_struct *exec(char *path, char *argv[], char *envp[]) ;
