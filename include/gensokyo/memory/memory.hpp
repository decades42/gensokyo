#pragma once

#include <string_view>
#include "address.hpp"
#include <vector>
#include <memory>

namespace gensokyo
{
    class Memory
    {
      public:
        template <size_t Index>
        static gensokyo::Address get_vfunc_address(gensokyo::Address vtable)
        {
            return (vtable.deref().cast<uintptr_t*>())[Index];
        }

#ifdef WINDOWS
        template <typename T, size_t Index, typename... Args>
        static constexpr T virtual_call(void* vtable, Args... args)
        {
            using fn = T(__thiscall*)(void*, decltype(args)...);
            return get_vfunc_address<Index>(vtable).template cast<fn>()(vtable, args...);
        }

        template <typename T, size_t Index, typename... Args>
        static constexpr T virtual_ccall(void* vtable, Args... args)
        {
            using fn = T(__cdecl*)(void*, decltype(args)...);
            return get_vfunc_address<Index>(vtable).template cast<fn>()(vtable, args...);
        }
#elif LINUX
        template <typename T, size_t Index, typename... Args>
        static constexpr T virtual_call(void* vtable, Args... args)
        {
            using fn = T (*)(void*, decltype(args)...);
            return get_vfunc_address<Index>(vtable).template cast<fn>()(vtable, args...);
        }
#endif
    };
}