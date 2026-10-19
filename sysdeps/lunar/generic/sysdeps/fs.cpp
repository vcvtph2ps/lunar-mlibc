#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <sysdeps/lunar/generic/syscall-helpers.h>

namespace mlibc {

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
int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) { STUB(); }
int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	STUB_WARN();
	return ENOSYS;
}
}; // namespace mlibc
