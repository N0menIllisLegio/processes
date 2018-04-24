#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, const char * argv[]) {
    pid_t pid;       /* идентификатор процесса */
    printf ("Пока всего один процесс\n");
    pid = fork ();     /* Создание нового процесса */
    printf ("Уже два процесса\n");
    wait(1);
    if (pid == 0){
        printf ("Это Дочерний процесс его pid=%d\n", getpid());
        printf ("А pid его Родительского процесса=%d\n", getppid());
    }
    else if (pid > 0)
        printf ("Это Родительский процесс pid=%d\n", getpid());
    else
        printf ("Ошибка вызова fork, потомок не создан\n");
    return 1;
}
