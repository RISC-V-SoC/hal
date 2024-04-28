ENTRY(__start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x100000, LENGTH = 0x20000
    SRAM (rwx) : ORIGIN = 0x120000, LENGTH = 0x40000
}

SECTIONS
{
    .text :
    {
        *(.text.startup)
        *(.text*)
        _flash_data = .;
        . = ALIGN(4);
    } > FLASH

    .data :
    {
        _data = .;
        *(vtable)
        *(.*data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.*bss*)
        _ebss = .;
    } > SRAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;
    } > SRAM

    _stack_start = ORIGIN(SRAM) + LENGTH(SRAM);
    _global_pointer = ORIGIN(SRAM) + LENGTH(SRAM)/2;
}
