#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void process_line(char *cmd, char *args[], int argc) {
    args[argc] = malloc(128);

    char buf;
    int i = 0;
    while (read(0, &buf, 1)) {
        if (buf == '\n') {
            args[argc][i] = '\0';
            if (fork() == 0) {
                exec(cmd, args);
            } else {
                i = 0;
                wait(0);
            }
        } else {
            args[argc][i++] = buf;
        }
    }
}

int main(int argc, char *argv[]) {
    int index = 0;
    int arg_count = 1;
    char *args[MAXARG];
    
    if (strcmp(argv[1], "-n") == 0) {
        index = 3;
    } else {
        index = 1;
    }
    
    args[0] = malloc(strlen(argv[index]) + 1);
    strcpy(args[0], argv[index]);
    
    for (int i = index + 1; i < argc; ++i) {
        args[arg_count] = malloc(strlen(argv[i]) + 1);
        strcpy(args[arg_count++], argv[i]);
    }
    
    process_line(argv[index], args, arg_count);

    exit(0);
}
