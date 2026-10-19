#include <sys/mount.h>
#include <lunar/syscall.h>
#include <errno.h>
#include <stddef.h>

int mount(const char *type, const char *dir, int flags, void *data) {
	syscall_result res = syscall(SYSCALL_FS_MOUNT, (size_t)type, (size_t)dir, flags, (size_t)data);
	if(res.is_error) {
    	errno = res.value;
    	return -1;
    }
    return 0;
}

int unmount(const char *dir, int flags) {
	syscall_result res = syscall(SYSCALL_FS_UNMOUNT, (size_t)dir, flags);
	if(res.is_error) {
    	errno = res.value;
    	return -1;
    }
    return 0;
}
