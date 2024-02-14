#include "kernel/types.h"
#include "user/user.h"

#define READ_END 0
#define WRITE_END 1

void child_process(int read_fd, int prime) {
    int pipe_fds[2];
    pipe(pipe_fds);

    int pid = fork();
    if (pid == 0) {
        close(pipe_fds[READ_END]);
        while (1) {
            int num;
            if (read(read_fd, &num, sizeof(int)) == 0) {
                exit(0);
            }
            if (num % prime != 0) {
                write(pipe_fds[WRITE_END], &num, sizeof(int));
            }
        }
    } else {
        close(read_fd);
        close(pipe_fds[WRITE_END]);
        child_process(pipe_fds[0], prime);
    }
}

int main() {
    int pipe_fds[2];
    pipe(pipe_fds);

    int pid = fork();
    if (pid == 0) {
        close(pipe_fds[READ_END]);
        for (int i = 2; i <= 35; i++) {
            write(pipe_fds[WRITE_END], &i, sizeof(int));
        }
        close(pipe_fds[WRITE_END]);
        exit(0);
    } else {
        close(pipe_fds[WRITE_END]);
        int prime;
        if (read(pipe_fds[READ_END], &prime, sizeof(int)) != 0) {
            printf("prime %d\n", prime);
            child_process(pipe_fds[READ_END], prime);
        }
        close(pipe_fds[READ_END]);
        wait(0);
    }
    exit(0);
}
