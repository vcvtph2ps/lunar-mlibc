#include "abi-bits/fcntl.h"
#include "frg/logging.hpp"
#include "lunar/syscall.h"
#include "mlibc/debug.hpp"
#include "mlibc/fsfd_target.hpp"
#include "mlibc/sysdep-tags.hpp"
#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	SYSCALL_OUT_OR_ERROR(fd, SYSCALL_FS_OPEN, (uintptr_t)pathname, flags, mode);
}

int Sysdeps<OpenDir>::operator()(const char *path, int *fd) {
	SYSCALL_OUT_OR_ERROR(fd, SYSCALL_FS_OPEN, (uintptr_t)path, O_RDONLY | O_DIRECTORY, 0600);
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
int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	SYSCALL_OR_ERROR(SYSCALL_FS_GETCWD, (uintptr_t)buffer, size);
}
int Sysdeps<Chdir>::operator()(const char *path) {
	SYSCALL_OR_ERROR(SYSCALL_FS_CHDIR, (uintptr_t)path);
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	__ensure(flags == 0);
	SYSCALL_OUT_OR_ERROR(newfd, SYSCALL_FS_DUP, fd, flags);
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	SYSCALL_OR_ERROR(SYSCALL_FS_DUP2, fd, flags, newfd);
}

int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	syscall_result res;

	switch (fsfdt) {
		case mlibc::fsfd_target::fd:
			res = syscall(SYSCALL_FS_STAT, fd, (uintptr_t)statbuf, flags);
			break;
		case mlibc::fsfd_target::path:
			res = syscall(SYSCALL_FS_STAT_AT, AT_FDCWD, (uintptr_t)path, (uintptr_t)statbuf, flags);
			break;
		case mlibc::fsfd_target::fd_path:
			res = syscall(SYSCALL_FS_STAT_AT, fd, (uintptr_t)path, (uintptr_t)statbuf, flags);
			break;
		default:
			mlibc::infoLogger() << "mlibc: stat: Unknown fsds_target: " << (int)fsfdt
			                    << frg::endlog;
			return ENOSYS;
	}

	if (res.is_error) {
		return res.value;
	}

	return res.value;
}

}; // namespace mlibc
