#pragma once

#include <bits/ensure.h>
#include <lunar/syscall.h>

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

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

#define STUB_WARN()                                                                                \
	({ __ensure_warn("STUB function was called", __FILE__, __LINE__, __PRETTY_FUNCTION__); })
