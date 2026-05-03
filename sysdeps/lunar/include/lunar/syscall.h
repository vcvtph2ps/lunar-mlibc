#ifndef _LUNAR_SYSCALL_H
#define _LUNAR_SYSCALL_H

#include <stdint.h>

#define SYSCALL_EXIT 0
#define SYSCALL_DEBUG_LOG 1
#define SYSCALL_TCB_SET 2

#define SYSCALL_VM_MAP 10
#define SYSCALL_VM_UNMAP 11
#define SYSCALL_VM_PROTECT 12

#define SYSCALL_OPEN 20
#define SYSCALL_CLOSE 21
#define SYSCALL_READ 22
#define SYSCALL_WRITE 23
#define SYSCALL_SEEK 24
#define SYSCALL_ISATTY 25
#define SYSCALL_GET_CWD 26
#define SYSCALL_STAT 27
#define SYSCALL_STAT_AT 28

#define SYSCALL_FUTEX 30
#define SYSCALL_CLOCK_GET 31
#define SYSCALL_GET_PROCESS_INFO 32

#define SYSCALL_GET_PROCESS_INFO_PID 0
#define SYSCALL_GET_PROCESS_INFO_PPID 1
#define SYSCALL_GET_PROCESS_INFO_GID 2
#define SYSCALL_GET_PROCESS_INFO_EGID 3
#define SYSCALL_GET_PROCESS_INFO_UID 4
#define SYSCALL_GET_PROCESS_INFO_EUID 5
#define SYSCALL_GET_PROCESS_INFO_GET_PGID 6
#define SYSCALL_GET_PROCESS_INFO_SET_PGID 7

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

#ifndef __MLIBC_ABI_ONLY

static long syscall(
    long func,
    long *ret,
    uint64_t p1 = 0,
    uint64_t p2 = 0,
    uint64_t p3 = 0,
    uint64_t p4 = 0,
    uint64_t p5 = 0,
    uint64_t p6 = 0
) {
	volatile long err;

	register uint64_t r4 asm("r10") = p4;
	register uint64_t r5 asm("r8") = p5;
	register uint64_t r6 asm("r9") = p6;

	asm volatile("syscall"
	             : "=a"(*ret), "=d"(err)
	             : "a"(func), "D"(p1), "S"(p2), "d"(p3), "r"(r4), "r"(r5), "r"(r6)
	             : "memory", "rcx", "r11", "r15");
	return err;
}

#endif /* !__MLIBC_ABI_ONLY */

#endif /* _LUNAR_SYSCALL_H */
