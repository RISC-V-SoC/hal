.global getCycleCount
.global getSystemTimeUs
.global getInstructionsRetiredCount

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
