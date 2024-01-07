.global getSystemTimeUs

getSystemTimeUs:
    rdtimeh a1
    rdtime a0
    rdtimeh a4
    bne a1, a4, getSystemTimeUs
    ret
