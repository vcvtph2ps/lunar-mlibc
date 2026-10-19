#include <mlibc/all-sysdeps.hpp>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {
int Sysdeps<VmMap>::operator()(
    void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window
) {
	// @note: we cast to a uintptr_t* from a void** because macro funny :^)
	SYSCALL_OUT_OR_ERROR(
	    (uintptr_t *)window, SYSCALL_VM_MAP, (uintptr_t)hint, size, prot, flags, fd, offset
	);
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	SYSCALL_OR_ERROR(SYSCALL_VM_UNMAP, (uintptr_t)pointer, size);
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	SYSCALL_OR_ERROR(SYSCALL_VM_PROTECT, (uintptr_t)pointer, size, prot);
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	size += 4096 - (size % 4096);
	return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0, pointer);
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	size += 4096 - (size % 4096);
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) { STUB(); }

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	STUB();
}
}; // namespace mlibc
