#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct LunarSysdepTags : LibcPanic,
                         LibcLog,
                         Isatty,
                         Write,
                         TcbSet,
                         AnonAllocate,
                         AnonFree,
                         Seek,
                         Exit,
                         Close,
                         FutexWake,
                         FutexWait,
                         Read,
                         Open,
                         VmMap,
                         VmUnmap,
                         ClockGet,
                         Dup2,
                         Stat,
                         VmProtect {};

template <typename Tag>
using Sysdeps = SysdepOf<LunarSysdepTags, Tag>;

} // namespace mlibc
