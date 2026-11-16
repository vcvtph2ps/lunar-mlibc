
#include <cstdint>
#include <errno.h>
#include <lunar/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <string.h>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	syscall(SYSCALL_PROC_EXIT, status);
	__builtin_unreachable();
}

void Sysdeps<LibcLog>::operator()(const char *message) {
	syscall(SYSCALL_SYS_DEBUG_LOG, (uintptr_t)message);
}

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc: panic");
	sysdep<Exit>(1);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
#if defined(__riscv) && __riscv_xlen == 64
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(thread_data));
	return 0;
#else
	SYSCALL_OR_ERROR(SYSCALL_SYS_TCB_SET, (uintptr_t)pointer);
#endif
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	STUB_WARN();
	return ENOSYS;
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
	SYSCALL_OR_ERROR(SYSCALL_SYS_UNAME, (uintptr_t)buf);
}

} // namespace mlibc
