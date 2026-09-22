#ifndef _LUNAR_SYSCALL_H
#define _LUNAR_SYSCALL_H

#include <stdint.h>

#define SYSCALL_PROC_EXIT 0

#define SYSCALL_PROC_GETINFO_GROUP_ID 0
#define SYSCALL_PROC_GETINFO_EGROUP_ID 1
#define SYSCALL_PROC_GETINFO_USER_ID 2
#define SYSCALL_PROC_GETINFO_EUSER_ID 3
#define SYSCALL_PROC_GETINFO_PROCESS_ID 4
#define SYSCALL_PROC_GETINFO_PARENT_PROCESS 5
#define SYSCALL_PROC_GETINFO_THREAD_ID 6

#define SYSCALL_PROC_GETINFO 1

#define SYSCALL_SYS_DEBUG_LOG 10
#define SYSCALL_SYS_TCB_SET 11

#define SYSCALL_VM_MAP 20
#define SYSCALL_VM_UNMAP 21
#define SYSCALL_VM_PROTECT 22

#define SYSCALL_FS_OPEN 30
#define SYSCALL_FS_CLOSE 31
#define SYSCALL_FS_READ 32
#define SYSCALL_FS_WRITE 33
#define SYSCALL_FS_SEEK 34
#define SYSCALL_FS_ISATTY 35

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
