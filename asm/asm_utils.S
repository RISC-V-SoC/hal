.global getCycleCount
.global getSystemTimeUs
.global getInstructionsRetiredCount
.global enableMachineTimerInterrupt
.global disableMachineTimerInterrupt
.global enableGlobalMachineInterrupt
.global disableGlobalMachineInterrupt
.global getHeartId

getCycleCount:
    rdcycleh a1
    rdcycle a0
    rdcycleh a4
    bne a1, a4, getCycleCount
    ret

getSystemTimeUs:
    rdtimeh a1
    rdtime a0
    rdtimeh a4
    bne a1, a4, getSystemTimeUs
    ret

getInstructionsRetiredCount:
    rdinstreth a1
    rdinstret a0
    rdinstreth a4
    bne a1, a4, getInstructionsRetiredCount
    ret

.option arch, +zicsr

enableMachineTimerInterrupt:
    li      a0, 0x80
    csrs    mie, a0
    ret

disableMachineTimerInterrupt:
    li      a0, 0x80
    csrc    mie, a0
    ret

enableGlobalMachineInterrupt:
    li      a0, 0x8
    csrs    mstatus, a0
    ret

disableGlobalMachineInterrupt:
    li      a0, 0x8
    csrc    mstatus, a0
    ret

getHeartId:
    csrr a0, mhartid
    ret
