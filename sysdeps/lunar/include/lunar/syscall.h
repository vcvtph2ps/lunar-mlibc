#ifndef _LUNAR_SYSCALL_H
#define _LUNAR_SYSCALL_H

#if !defined(__ASSEMBLER__)
#include <stdint.h>
#endif

#define SYSCALL_PROC_EXIT 0

// These are defined for SYSCALL_PROC_GETINFO & SYSCALL_PROC_SETINFO
#define SYSCALL_PROC_INFO_GROUP_ID 0
#define SYSCALL_PROC_INFO_EGROUP_ID 1
#define SYSCALL_PROC_INFO_USER_ID 2
#define SYSCALL_PROC_INFO_EUSER_ID 3
#define SYSCALL_PROC_INFO_PROCESS_ID 4
#define SYSCALL_PROC_INFO_PARENT_PROCESS 5
#define SYSCALL_PROC_INFO_THREAD_ID 6
#define SYSCALL_PROC_INFO_PROCESS_GROUP_ID 7

#define SYSCALL_PROC_GETINFO 1
#define SYSCALL_PROC_SETINFO 2

#define SYSCALL_PROC_FORK 3
#define SYSCALL_PROC_WAITPID 4
#define SYSCALL_PROC_EXECVE 5
#define SYSCALL_PROC_FEXECVE 6

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
#define SYSCALL_FS_MOUNT 46
#define SYSCALL_FS_UNMOUNT 47

#if !defined(__ASSEMBLER__)

#if defined(__x86_64__)
#define LUNAR_ASM_REG_NUM "rax"
#define LUNAR_ASM_REG_RET "rax"
#define LUNAR_ASM_REG_ERR "rdx"
#define LUNAR_ASM_REG_A0 "rdi"
#define LUNAR_ASM_REG_A1 "rsi"
#define LUNAR_ASM_REG_A2 "rdx"
#define LUNAR_ASM_REG_A3 "r10"
#define LUNAR_ASM_REG_A4 "r8"
#define LUNAR_ASM_REG_A5 "r9"
#define LUNAR_ASM_SYSCALL "syscall"
#define LUNAR_ASM_CLOBBER "memory", "rcx", "r11", "r15"
#elif defined(__riscv) && (__riscv_xlen == 64)
#define LUNAR_ASM_REG_NUM "a7"
#define LUNAR_ASM_REG_RET "a0"
#define LUNAR_ASM_REG_ERR "a1"
#define LUNAR_ASM_REG_A0 "a0"
#define LUNAR_ASM_REG_A1 "a1"
#define LUNAR_ASM_REG_A2 "a2"
#define LUNAR_ASM_REG_A3 "a3"
#define LUNAR_ASM_REG_A4 "a4"
#define LUNAR_ASM_REG_A5 "a5"
#define LUNAR_ASM_SYSCALL "ecall"
#define LUNAR_ASM_CLOBBER "memory"
#endif

struct syscall_result {
	uint64_t value;
	uint64_t is_error;
};
static_assert(sizeof(syscall_result) == 16);

#if !defined(__MLIBC_ABI_ONLY)

static inline syscall_result
syscall6(long func, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5) {
	register uint64_t rfunc asm(LUNAR_ASM_REG_NUM) = func;
	register uint64_t value asm(LUNAR_ASM_REG_RET);
	register uint64_t err asm(LUNAR_ASM_REG_ERR);
	register uint64_t r0 asm(LUNAR_ASM_REG_A0) = a0;
	register uint64_t r1 asm(LUNAR_ASM_REG_A1) = a1;
	register uint64_t r2 asm(LUNAR_ASM_REG_A2) = a2;
	register uint64_t r3 asm(LUNAR_ASM_REG_A3) = a3;
	register uint64_t r4 asm(LUNAR_ASM_REG_A4) = a4;
	register uint64_t r5 asm(LUNAR_ASM_REG_A5) = a5;
	asm volatile(LUNAR_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rfunc), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
	             : LUNAR_ASM_CLOBBER);

	return {.value = value, .is_error = err};
}

static inline syscall_result
syscall5(long func, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
	return syscall6(func, a0, a1, a2, a3, a4, 0);
}

static inline syscall_result
syscall4(long func, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
	return syscall6(func, a0, a1, a2, a3, 0, 0);
}

static inline syscall_result syscall3(long func, uint64_t a0, uint64_t a1, uint64_t a2) {
	return syscall6(func, a0, a1, a2, 0, 0, 0);
}

static inline syscall_result syscall2(long func, uint64_t a0, uint64_t a1) {
	return syscall6(func, a0, a1, 0, 0, 0, 0);
}

static inline syscall_result syscall1(long func, uint64_t a0) {
	return syscall6(func, a0, 0, 0, 0, 0, 0);
}

static inline syscall_result syscall0(long func) { return syscall6(func, 0, 0, 0, 0, 0, 0); }

#if defined(__cplusplus)
static inline syscall_result syscall(
    long func,
    uint64_t a0 = 0,
    uint64_t a1 = 0,
    uint64_t a2 = 0,
    uint64_t a3 = 0,
    uint64_t a4 = 0,
    uint64_t a5 = 0
) {
	return syscall6(func, a0, a1, a2, a3, a4, a5);
}
#endif

#endif /* !__MLIBC_ABI_ONLY */
#endif /* !__ASSEMBLER__ */
#endif /* _LUNAR_SYSCALL_H */
