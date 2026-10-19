#include <mlibc/all-sysdeps.hpp>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

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

}; // namespace mlibc
