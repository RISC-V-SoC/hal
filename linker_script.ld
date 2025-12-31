ENTRY(_start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x100000, LENGTH = 0x20000
    SRAM (rwx) : ORIGIN = 0x120000, LENGTH = 0x40000
}

SECTIONS
{
    .text (READONLY) :
    {
        __text_start = .;
        *(.text.initial)
        *(.text .text.* .gnu.linkonce.t.*)
        __text_end = .;
    } > FLASH

    .rodata (READONLY) :
    {
        __rodata_start = .;
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.gcc_except_table .gcc_except_table.*)
        *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)

        . = ALIGN(4);

        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        __init_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        __init_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP (*(.fini_array))
        __init_array_end = .;

    } > FLASH

    PROVIDE(__ramfunc_in_flash_start = .);

    .ramfunc :
    {
        __ramfunc_start = .;
        *(.ramfunc .ramfunc.*);
        __ramfunc_end = .;
    } > SRAM AT >FLASH

    PROVIDE (__data_in_flash_start = __ramfunc_in_flash_start + SIZEOF(.ramfunc));

    .data :
    {
        . = ALIGN(4);
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        . = ALIGN(4);
        __data_end = .;
    } > SRAM AT > FLASH

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _bss = .;
        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)
        *(.bss .bss.*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;
    } > SRAM

    _stack_start = ORIGIN(SRAM) + LENGTH(SRAM);
    __global_pointer$ = __rodata_start + 0x800;
}
