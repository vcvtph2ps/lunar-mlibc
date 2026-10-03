
#include <bits/ensure.h>
#include <cstdint>
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

#define SYSCALL_OR_ERROR(...)                                                                      \
	syscall_result res = syscall(__VA_ARGS__);                                                     \
	if (res.is_error) {                                                                            \
		return res.value;                                                                          \
	}                                                                                              \
	return 0;

#define SYSCALL_NO_ERROR_RET_VAL(...)                                                              \
	syscall_result res = syscall(__VA_ARGS__);                                                     \
	__ensure(!res.is_error && "infallible syscall failed");                                        \
	return res.value;

#define SYSCALL_OUT_OR_ERROR(x, ...)                                                               \
	syscall_result res = syscall(__VA_ARGS__);                                                     \
	if (res.is_error) {                                                                            \
		return res.value;                                                                          \
	}                                                                                              \
	(*x) = res.value;                                                                              \
	return 0;

int Sysdeps<TcbSet>::operator()(void *pointer) {
	SYSCALL_OR_ERROR(SYSCALL_SYS_TCB_SET, (uintptr_t)pointer);
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	SYSCALL_OUT_OR_ERROR(fd, SYSCALL_FS_OPEN, (uintptr_t)pathname, strlen(pathname), flags, mode);
}

int Sysdeps<Read>::operator()(int fd, void *buff, size_t count, ssize_t *bytes_read) {
	SYSCALL_OUT_OR_ERROR(bytes_read, SYSCALL_FS_READ, fd, (uintptr_t)buff, count);
}

int Sysdeps<Write>::operator()(int fd, const void *buff, size_t count, ssize_t *bytes_written) {
	SYSCALL_OUT_OR_ERROR(bytes_written, SYSCALL_FS_WRITE, fd, (uintptr_t)buff, count);
}

int Sysdeps<Close>::operator()(int fd) { SYSCALL_OR_ERROR(SYSCALL_FS_CLOSE, fd); }

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	SYSCALL_OUT_OR_ERROR(new_offset, SYSCALL_FS_SEEK, fd, offset, whence);
}

int Sysdeps<Isatty>::operator()(int fd) { SYSCALL_OR_ERROR(SYSCALL_FS_ISATTY, fd); }

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
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_GROUP_ID);
}

gid_t Sysdeps<GetEgid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_EGROUP_ID);
}

uid_t Sysdeps<GetUid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_USER_ID);
}

uid_t Sysdeps<GetEuid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_EUSER_ID);
}

pid_t Sysdeps<GetPid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_PROCESS_ID);
}

pid_t Sysdeps<GetPpid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_PARENT_PROCESS);
}

pid_t Sysdeps<GetTid>::operator()() {
	SYSCALL_NO_ERROR_RET_VAL(SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_THREAD_ID);
}

pid_t Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	SYSCALL_OUT_OR_ERROR(
	    pgid, SYSCALL_PROC_GETINFO, UINT64_MAX, SYSCALL_PROC_INFO_PROCESS_GROUP_ID
	);
}

pid_t Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	SYSCALL_NO_ERROR_RET_VAL(
	    SYSCALL_PROC_SETINFO, UINT64_MAX, SYSCALL_PROC_INFO_PROCESS_GROUP_ID, pgid
	);
}

int Sysdeps<Fork>::operator()(pid_t *pid) { SYSCALL_OUT_OR_ERROR(pid, SYSCALL_PROC_FORK); }
int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_EXECVE, (uintptr_t)path, (uintptr_t)argv, (uintptr_t)envp);
}
int Sysdeps<Fexecve>::operator()(int fd, char *const argv[], char *const envp[]) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_FEXECVE, fd, (uintptr_t)argv, (uintptr_t)envp);
}

int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_SEND_SIMPLE, pid, UINT64_MAX, signal);
}

int Sysdeps<Tgkill>::operator()(int pid, int tid, int signal) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_SEND_SIMPLE, pid, tid, signal);
}

int Sysdeps<Sigprocmask>::operator()(
    int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve
) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_MASK, how, (uintptr_t)set, (uintptr_t)retrieve);
}

int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_ALT_STACK, (uintptr_t)ss, (uintptr_t)oss);
}

int Sysdeps<Sigtimedwait>::operator()(
    const sigset_t *__restrict set,
    siginfo_t *__restrict info,
    const struct timespec *__restrict timeout,
    int *out_signal
) {
	SYSCALL_OUT_OR_ERROR(
	    out_signal, SYSCALL_PROC_SIG_WAIT, (uintptr_t)set, (uintptr_t)info, (uintptr_t)timeout
	);
}

int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_SUSPEND, (uintptr_t)set);
}

int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_PENDING, (uintptr_t)set);
}

int Sysdeps<Sigqueue>::operator()(pid_t pid, int sig, const union sigval val) {
	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_QUEUE, pid, sig, (size_t)val.sival_ptr);
}

int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if (ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}

again:
	auto r = syscall(SYSCALL_PROC_WAITPID, pid, (size_t)status, flags, (size_t)ru);
	if (r.is_error) {
		if (r.value == EINTR)
			goto again;
		return r.value;
	}
	*ret_pid = (pid_t)r.value;
	return 0;
}

#if !defined(MLIBC_BUILDING_RTLD)
extern "C" void __mlibc_restorer();

int Sysdeps<Sigaction>::operator()(
    int sig, const struct sigaction *__restrict act, struct sigaction *__restrict oldact
) {
	struct sigaction action;
	if (act != nullptr) {
		memcpy(&action, act, sizeof(struct sigaction));
		action.sa_restorer = __mlibc_restorer;
	}

	SYSCALL_OR_ERROR(SYSCALL_PROC_SIG_ACTION, sig, act ? (uintptr_t)&action : 0, (uintptr_t)oldact);
}
#endif

} // namespace mlibc
