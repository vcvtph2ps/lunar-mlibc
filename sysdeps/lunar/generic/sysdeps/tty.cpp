#include "abi-bits/fcntl.h"
#include "abi-bits/ioctls.h"
#include "abis/linux/termios.h"
#include "frg/logging.hpp"
#include "lunar/syscall.h"
#include "mlibc/debug.hpp"
#include "mlibc/fsfd_target.hpp"
#include "mlibc/sysdep-tags.hpp"
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <sysdeps/lunar/generic/syscall-helpers.h>
namespace mlibc {

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	return sysdep<Ioctl>(fd, TCGETS, attr, nullptr);
}

int Sysdeps<Tcsetattr>::operator()(int fd, int opts, const struct termios *attr) {
	int req;
	switch (opts) {
		case TCSANOW:
			req = TCSETS;
			break;
		case TCSADRAIN:
			req = TCSETSW;
			break;
		case TCSAFLUSH:
			req = TCSETSF;
			break;
		default:
			return EINVAL;
	}

	return sysdep<Ioctl>(fd, req, const_cast<struct termios *>(attr), nullptr);
}

int Sysdeps<Isatty>::operator()(int fd) {
	struct winsize ws;
	if (!sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, nullptr))
		return 0;
	return ENOTTY;
}
}; // namespace mlibc
