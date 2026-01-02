.global syncTrapHandler_asm
.option arch, +zicsr

syncTrapHandler_asm:
.option push
.option norelax
    # Reset stack pointer
    la sp, _stack_start
.option pop
    csrr a0, mcause
    csrr a1, mepc
    call syncTrapHandler
_end:
    j _end
