#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MSTATUS_MIE (1u << 3)
#define MIE_MTIE (1u << 7)
#define MIE_MEIE (1u << 11)

// Machine external interrupt disable/enable
#define MEI_DIS                                  \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrc mie, %0"                       \
            :                                    \
            : "r"(MIE_MEIE)                      \
            :                                    \
        );                                       \
    } while (0)
#define MEI_EN                                   \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrs mie, %0"                       \
            :                                    \
            : "r"(MIE_MEIE)                      \
            :                                    \
        );                                       \
    } while (0)

// Machine timer interrupt disable/enable
#define MTI_DIS                                  \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrc mie, %0"                       \
            :                                    \
            : "r"(MIE_MTIE)                      \
            :                                    \
        );                                       \
    } while (0)
#define MTI_EN                                   \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrs mie, %0"                       \
            :                                    \
            : "r"(MIE_MTIE)                      \
            :                                    \
        );                                       \
    } while (0)

// Machine global interrupt disable/enable
#define MGI_DIS                                  \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrc mstatus, %0"                   \
            :                                    \
            : "r"(MSTATUS_MIE)                   \
            :                                    \
        );                                       \
    } while (0)

#define MGI_EN                                   \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrs mstatus, %0"                   \
            :                                    \
            : "r"(MSTATUS_MIE)                   \
            :                                    \
        );                                       \
    } while (0)

#ifdef __cplusplus
}
#endif
