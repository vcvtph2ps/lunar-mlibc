#include <lunar/syscall.h>
.section .text
.global __mlibc_restorer

__mlibc_restorer:
    mov $SYSCALL_PROC_SIG_RETURN, %rax
    syscall
    ud2

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
