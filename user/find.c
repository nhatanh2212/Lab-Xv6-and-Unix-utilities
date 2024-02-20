#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *dir_path, char *target_file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(dir_path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot find path %s\n", dir_path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir_path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", dir_path);
        close(fd);
        return;
    }

    if (strlen(dir_path) + 1 + DIRSIZ + 1 > sizeof buf){
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, dir_path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            fprintf(1, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_FILE && !strcmp(de.name, target_file)) {
            fprintf(1, "%s\n", buf);
        } else if (st.type == T_DIR) {
            find(buf, target_file);
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(2, "Usage: find (path) [file]\n");
        exit(1);
    } else if (argc < 3) {
        find(".", argv[1]);
    } else {
        find(argv[1], argv[2]);
    }

    exit(0);
}
