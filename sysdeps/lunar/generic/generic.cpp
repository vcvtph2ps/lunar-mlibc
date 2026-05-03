
#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
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
	syscall(SYSCALL_EXIT, NULL, status);
	__builtin_unreachable();
}

void Sysdeps<LibcLog>::operator()(const char *message) {
	long ret;
	syscall(SYSCALL_DEBUG_LOG, &ret, (uint64_t)message, strlen(message));
}

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc: panic");
	sysdep<Exit>(1);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	long ret;
	syscall(SYSCALL_TCB_SET, &ret, (uint64_t)pointer);
	return ret;
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	long ret;
	bool err = syscall(SYSCALL_OPEN, &ret, (uintptr_t)pathname, strlen(pathname), flags, mode);
	if (err) {
		return ret;
	}
	*fd = ret;
	return 0;
};
int Sysdeps<Read>::operator()(int fd, void *buff, size_t count, ssize_t *bytes_read) {
	long ret;
	bool err = syscall(SYSCALL_READ, &ret, fd, (uintptr_t)buff, count);
	if (err) {
		return ret;
	}
	*bytes_read = ret;
	return 0;
}
int Sysdeps<Write>::operator()(int fd, const void *buff, size_t count, ssize_t *bytes_written) {
	long ret;
	bool err = syscall(SYSCALL_WRITE, &ret, fd, (uintptr_t)buff, count);
	if (err) {
		return ret;
	}
	*bytes_written = ret;
	return 0;
}
int Sysdeps<Close>::operator()(int fd) {
	long ret;
	bool err = syscall(SYSCALL_CLOSE, &ret, fd);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	long ret;
	bool err = syscall(SYSCALL_SEEK, &ret, fd, offset, whence);
	if (err) {
		return ret;
	}
	*new_offset = ret;
	return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
	long ret;
	bool err = syscall(SYSCALL_ISATTY, &ret, fd);
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

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	struct timespec ts;

	long ret;
	bool err = syscall(SYSCALL_CLOCK_GET, &ret, clock, (uint64_t)&ts);
	if (err) {
		return ret;
	}

	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	long ret;
	bool err = syscall(
	    SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAIT, (uint64_t)expected, (uint64_t)time
	);
	if (err) {
		return ret;
	}
	return 0;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	long ret;
	int32_t val = all ? INT32_MAX : 1;
	syscall(SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAKE, (uint64_t)val);
	return 0;
}

pid_t Sysdeps<GetPid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_PID);
	if (err) {
		return ret;
	}
	return ret;
}

gid_t Sysdeps<GetGid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_GID);
	if (err) {
		return ret;
	}
	return ret;
}
gid_t Sysdeps<GetEgid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_EGID);
	if (err) {
		return ret;
	}
	return ret;
}
uid_t Sysdeps<GetUid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_UID);
	if (err) {
		return ret;
	}
	return ret;
}
uid_t Sysdeps<GetEuid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_EUID);
	if (err) {
		return ret;
	}
	return ret;
}

pid_t Sysdeps<GetPpid>::operator()() {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_PPID);
	if (err) {
		return ret;
	}
	return ret;
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	long ret;
	bool err = syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_GET_PGID, pid);
	if (err) {
		return ret;
	}
	*pgid = ret;
	return 0;
}
int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	long ret;
	bool err =
	    syscall(SYSCALL_GET_PROCESS_INFO, &ret, SYSCALL_GET_PROCESS_INFO_SET_PGID, pid, pgid);
	if (err) {
		return ret;
	}
	return ret;
}
int Sysdeps<GetCwd>::operator()(char *buf, size_t size) {
	long ret;
	bool err = syscall(SYSCALL_GET_CWD, &ret, (uint64_t)buf, size);
	if (err) {
		return ret;
	}
	return 0;
}
int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) { STUB(); }
int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	long ret;
	bool err;
	switch (fsfdt) {
		case fsfd_target::path:
			err = syscall(
			    SYSCALL_STAT_AT,
			    &ret,
			    AT_FDCWD,
			    (uint64_t)path,
			    strlen(path),
			    (uint64_t)statbuf,
			    flags
			);
			break;
		case fsfd_target::fd:
			err = syscall(SYSCALL_STAT, &ret, fd, (uint64_t)statbuf);
			break;
		case fsfd_target::fd_path:
			err = syscall(
			    SYSCALL_STAT_AT, &ret, fd, (uint64_t)path, strlen(path), (uint64_t)statbuf, flags
			);
			break;
		default:
			mlibc::infoLogger() << "mlibc: stat: Unknown fsfd_target: " << (int)fsfdt
			                    << frg::endlog;
			return ENOSYS;
	}

	if (err) {
		return ret;
	}
	return 0;
}
} // namespace mlibc
