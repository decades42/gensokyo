#pragma once

#include <gensokyo/logger/logger.hpp>

#include <gensokyo/helper/bitflags.hpp>
#include <gensokyo/helper/cpu.hpp>
#include <gensokyo/helper/simd.hpp>

#include <gensokyo/memory/address.hpp>
#include <gensokyo/memory/memory.hpp>
#include <gensokyo/memory/module.hpp>
#include <gensokyo/memory/pattern.hpp>
#include <gensokyo/memory/process.hpp>
#if defined(WINDOWS)
    #include <gensokyo/memory/windows/win_process.hpp>
#elif defined(LINUX)
    #include <gensokyo/memory/linux/linux_process.hpp>
#endif

#include <gensokyo/math/angle.hpp>
#include <gensokyo/math/vectors.hpp>
#include <gensokyo/math/matrix.hpp>
#include <gensokyo/math/math_funcs.hpp>
