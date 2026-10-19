
#include <cstdint>
#include <errno.h>
#include <lunar/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	syscall(SYSCALL_PROC_EXIT, status);
	__builtin_unreachable();
}

void Sysdeps<LibcLog>::operator()(const char *message) {
	syscall(SYSCALL_SYS_DEBUG_LOG, (uintptr_t)message, strlen(message));
}

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc: panic");
	sysdep<Exit>(1);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	SYSCALL_OR_ERROR(SYSCALL_SYS_TCB_SET, (uintptr_t)pointer);
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	STUB_WARN();
	return ENOSYS;
}

} // namespace mlibc
