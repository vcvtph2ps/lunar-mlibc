#include <lunar/syscall.h>

.global __mlibc_restorer

__mlibc_restorer:
    li a7, SYSCALL_PROC_SIG_RETURN
    ecall
    unimp

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
