#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

void error(char* msg) {
    perror(msg);
    exit(1);
}

void Usage(){
    fprintf(stderr, "Uso: ./buscador <comando> <palabra>\n");
    exit(1);
}

int main(int argc, char* argv[]) {

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGCHLD);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    if (argc < 3) {
        Usage();
    }

    int pid_hijo, pid_hijo2;
    int fd[2];

    if (pipe(fd) == -1) {
        error("pipe");
    }

    pid_hijo = fork();

    if (pid_hijo == 0) {
        // En el proceso hijo 1
        dup2(fd[1], 1); // Redirigir stdout al pipe
        close(fd[1]);
        close(fd[0]); // Cerrar la lectura del pipe en este hijo
        execlp("man", "man", argv[1], (char*)NULL);
        error("execlp");
    } else if (pid_hijo < 0) {
        error("fork");
    }

    pid_hijo2 = fork();

    if (pid_hijo2 == 0) {
        // En el proceso hijo 2
        int out_fd = open("/home/omar/buscador_man/output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (out_fd < 0) {
            error("open");
        }
        dup2(fd[0], 0); // Redirigir stdin al pipe
        close(fd[0]);
        close(fd[1]); // Cerrar la escritura del pipe en este hijo
        dup2(out_fd, 1); // Redirigir stdout al archivo
        close(out_fd);
        execlp("grep", "grep", "-i",argv[2], (char*)NULL);
        error("execlp");
    } else if (pid_hijo2 < 0) {
        error("fork");
    }

    // Cerrar descriptores en el proceso padre
    close(fd[0]);
    close(fd[1]);

    for (int i = 0; i < 2; i++) {
        waitpid(-1, NULL, 0);
    }

    exit(0);
}