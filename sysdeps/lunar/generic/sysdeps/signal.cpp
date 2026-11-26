#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

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
}; // namespace mlibc
