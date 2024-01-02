#include <sys/stat.h>

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _lseek(int file, int offset, int whence) {
    (void)file;
    (void)offset;
    (void)whence;
    return 0;
}

int _close(int fd) {
    (void)fd;
    return -1;
}
int _write (int fd, char *buf, int count) {
    (void)fd;
    (void)buf;
    (void)count;
    return 0;
}

int _read (int fd, char *buf, int count) {
    (void)fd;
    (void)buf;
    (void)count;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

void _exit(int status) {
    (void)status;
    while(1);
}

void _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    return;
}

int _getpid(void) {
    return -1;
}
