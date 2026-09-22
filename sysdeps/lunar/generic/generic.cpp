
#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <lunar/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

#define STUB_WARN()                                                                                \
	({ __ensure_warn("STUB function was called", __FILE__, __LINE__, __PRETTY_FUNCTION__); })

namespace mlibc {

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	syscall(SYSCALL_PROC_EXIT, NULL, status);
	__builtin_unreachable();
}

void Sysdeps<LibcLog>::operator()(const char *message) {
	long ret;
	syscall(SYSCALL_SYS_DEBUG_LOG, &ret, (uint64_t)message, strlen(message));
}

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc: panic");
	sysdep<Exit>(1);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	long ret;
	syscall(SYSCALL_SYS_TCB_SET, &ret, (uint64_t)pointer);
	return ret;
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	long ret;
	bool err = syscall(SYSCALL_FS_OPEN, &ret, (uintptr_t)pathname, strlen(pathname), flags, mode);
	if (err) {
		return ret;
	}
	*fd = ret;
	return 0;
};
int Sysdeps<Read>::operator()(int fd, void *buff, size_t count, ssize_t *bytes_read) {
	long ret;
	bool err = syscall(SYSCALL_FS_READ, &ret, fd, (uintptr_t)buff, count);
	if (err) {
		return ret;
	}
	*bytes_read = ret;
	return 0;
}
int Sysdeps<Write>::operator()(int fd, const void *buff, size_t count, ssize_t *bytes_written) {
	long ret;
	bool err = syscall(SYSCALL_FS_WRITE, &ret, fd, (uintptr_t)buff, count);
	if (err) {
		return ret;
	}
	*bytes_written = ret;
	return 0;
}
int Sysdeps<Close>::operator()(int fd) {
	long ret;
	bool err = syscall(SYSCALL_FS_CLOSE, &ret, fd);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	long ret;
	bool err = syscall(SYSCALL_FS_SEEK, &ret, fd, offset, whence);
	if (err) {
		return ret;
	}
	*new_offset = ret;
	return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
	long ret;
	bool err = syscall(SYSCALL_FS_ISATTY, &ret, fd);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<VmMap>::operator()(
    void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window
) {
	long ret;
	bool err = syscall(SYSCALL_VM_MAP, &ret, (uint64_t)hint, size, prot, flags, fd, offset);
	if (err) {
		return ret;
	}
	*window = (void *)ret;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	long ret;
	bool err = syscall(SYSCALL_VM_UNMAP, &ret, (uint64_t)pointer, size);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	long ret;
	bool err = syscall(SYSCALL_VM_PROTECT, &ret, (uint64_t)pointer, size, prot);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	size += 4096 - (size % 4096);
	return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0, pointer);
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	size += 4096 - (size % 4096);
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	STUB();
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	STUB_WARN();
	return ENOSYS;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) { STUB(); }

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) { STUB(); }
int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	STUB_WARN();
	return ENOSYS;
}

gid_t Sysdeps<GetGid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_GROUP_ID);
	return ret;
}

gid_t Sysdeps<GetEgid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_EGROUP_ID);
	return ret;
}

uid_t Sysdeps<GetUid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_USER_ID);
	return ret;
}

uid_t Sysdeps<GetEuid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_EUSER_ID);
	return ret;
}

pid_t Sysdeps<GetPid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_PROCESS_ID);
	return ret;
}

pid_t Sysdeps<GetPpid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_PARENT_PROCESS);
	return ret;
}

pid_t Sysdeps<GetTid>::operator()() {
	long ret;
	syscall(SYSCALL_PROC_GETINFO, &ret, SYSCALL_PROC_GETINFO_THREAD_ID);
	return ret;
}

} // namespace mlibc
