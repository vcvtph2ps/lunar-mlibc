#ifndef _LUNAR_SYSCALL_H
#define _LUNAR_SYSCALL_H

#if !defined(__ASSEMBLER__)
#include <stdint.h>
#endif

#define SYSCALL_PROC_EXIT 0

#define SYSCALL_PROC_GETINFO_GROUP_ID 0
#define SYSCALL_PROC_GETINFO_EGROUP_ID 1
#define SYSCALL_PROC_GETINFO_USER_ID 2
#define SYSCALL_PROC_GETINFO_EUSER_ID 3
#define SYSCALL_PROC_GETINFO_PROCESS_ID 4
#define SYSCALL_PROC_GETINFO_PARENT_PROCESS 5
#define SYSCALL_PROC_GETINFO_THREAD_ID 6

#define SYSCALL_PROC_GETINFO 1
#define SYSCALL_PROC_FORK 2
#define SYSCALL_PROC_WAITPID 3
#define SYSCALL_PROC_EXECVE 4
#define SYSCALL_PROC_FEXECVE 5

#define SYSCALL_PROC_SIG_SEND_SIMPLE 10
#define SYSCALL_PROC_SIG_QUEUE 11

#define SYSCALL_PROC_SIG_ACTION 12
#define SYSCALL_PROC_SIG_MASK 13
#define SYSCALL_PROC_SIG_ALT_STACK 14
#define SYSCALL_PROC_SIG_RETURN 15

#define SYSCALL_PROC_SIG_SUSPEND 16
#define SYSCALL_PROC_SIG_WAIT 17
#define SYSCALL_PROC_SIG_PENDING 18

#define SYSCALL_SYS_DEBUG_LOG 20
#define SYSCALL_SYS_TCB_SET 21

#define SYSCALL_VM_MAP 30
#define SYSCALL_VM_UNMAP 31
#define SYSCALL_VM_PROTECT 32

#define SYSCALL_FS_OPEN 40
#define SYSCALL_FS_CLOSE 41
#define SYSCALL_FS_READ 42
#define SYSCALL_FS_WRITE 43
#define SYSCALL_FS_SEEK 44
#define SYSCALL_FS_ISATTY 45

#if !defined(__ASSEMBLER__)

struct syscall_result {
	uint64_t value;
	uint64_t is_error;
};
static_assert(sizeof(syscall_result) == 16);

#if !defined(__MLIBC_ABI_ONLY)

static inline syscall_result syscall(
    long func,
    uint64_t p1 = 0,
    uint64_t p2 = 0,
    uint64_t p3 = 0,
    uint64_t p4 = 0,
    uint64_t p5 = 0,
    uint64_t p6 = 0
) {
	volatile uint64_t err;
	volatile uint64_t ret;

	register uint64_t r4 asm("r10") = p4;
	register uint64_t r5 asm("r8") = p5;
	register uint64_t r6 asm("r9") = p6;

	asm volatile("syscall"
	             : "=a"(ret), "=d"(err)
	             : "a"(func), "D"(p1), "S"(p2), "d"(p3), "r"(r4), "r"(r5), "r"(r6)
	             : "memory", "rcx", "r11", "r15");
	return {.value = ret, .is_error = err};
}

#endif /* !__MLIBC_ABI_ONLY */
#endif /* !__ASSEMBLER__ */
#endif /* _LUNAR_SYSCALL_H */
