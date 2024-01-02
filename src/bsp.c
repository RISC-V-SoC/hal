#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "uart.h"

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
    for (int i = 0; i < count; ++i) {
        uart_putCharBlocking(buf[i]);
    }
    return count;
}

int _read (int fd, char *buf, int count) {
    /* Note that if _read ever returns 0, this fd will be marked as having reached eof
     * and any subsequent read will immidiately return without even trying. Therefore
     * be a blocking read.
     * Count will be some big number (1024), so waiting until we have enough characters
     * is usually not an option.
     */
    (void)fd;
    uint16_t available = 0;
    while (available == 0) {
        available = uart_getRxCharsAvailable();
    }
    int total = 0;
    while(available > 0 && count > 0) {
        *buf = uart_getCharBlocking();
        ++buf;
        ++total;
        --count;
        --available;
    }
    return total;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

void _exit(int status) {
    printf("Entering exit..\r\n");
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
