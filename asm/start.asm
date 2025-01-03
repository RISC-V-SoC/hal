.section .text.initial
.global _start
.option arch, +zicsr

_start:
.option push
.option norelax
    # Load stackpointer
    la      sp, _stack_start
    # Load global pointer
    la      gp, __global_pointer$
.option pop
    # Setup the trap vector
    la      a0, syncTrapHandler_asm
    csrw    mtvec,a0
    # Clear the bss segment
    la      a0, _bss
    la      a2, _ebss
    sub     a2, a2, a0
    li      a1, 0
    call    memset
    # Copy over ramfuncs
    la      a0, __ramfunc_start
    la      a1, __ramfunc_in_flash_start
    la      a2, __ramfunc_end
    sub     a2, a2, a0
    call    memcpy
    # Copy the data segment
    la      a0, __data_start
    la      a1, __data_in_flash_start
    la      a2, __data_end
    sub     a2, a2, a0
    call    memcpy

    call    __libc_init_array
    call    main
    tail    exit
