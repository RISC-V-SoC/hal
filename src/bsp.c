#include <sys/stat.h>

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _lseek(int file, int offset, int whence) {
  return 0;
}

int _close(int fd) {
  return -1;
}
int _write (int fd, char *buf, int count) {
  return 0;
}

int _read (int fd, char *buf, int count) {
  return 0;
}

int _isatty(int file) {
  return 1;
}

void _exit(int status) {
    return;
}

void _kill(int pid, int sig) {
  return;
}

int _getpid(void) {
  return -1;
}
