#include <string.h>
#include <stdlib.h>

extern char _bss;
extern char _ebss;

extern char _flash_data;
extern char _data;
extern char _edata;

void main(void);

void startup(void) __attribute__ ((noreturn));

__attribute__((section(".text.startup"))) void startup(void);
void startup(void) {
    size_t bss_size = &_ebss - &_bss;
    memset(&_bss, 0, bss_size);

    size_t data_size = &_edata - &_data;
    memcpy(&_data, &_flash_data, data_size);

    main();

    while(1);
}
