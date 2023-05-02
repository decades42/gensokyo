#pragma once

#include <cstdint>
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <functional>

namespace gensokyo::impl
{
    struct Segments
    {
        Segments() = default;

        Segments(std::uintptr_t address_, std::uint8_t* data_, std::size_t size_)
         : address(address_),
           data(data_, size_)
        {
        }

        Segments(const Segments&)            = default;
        Segments(Segments&&)                 = default;
        Segments& operator=(const Segments&) = default;
        Segments& operator=(Segments&&)      = default;

        std::uintptr_t address {};
        std::span<std::uint8_t> data {};
    };

    class Module
    {
      public:
        using FunctionCallbackFn = std::function<void(const std::vector<uint8_t>&)>;

      private:
        std::vector<Segments> _segments {};
        std::uintptr_t _baseAddress {};
        std::size_t _size {};

        void* _handle {};
        void get_module_nfo(std::string_view mod, const FunctionCallbackFn& func = nullptr);

      public:
        Module()                         = default;
        Module(const Module&)            = default;
        Module(Module&&)                 = default;
        Module& operator=(const Module&) = default;
        Module& operator=(Module&&)      = default;

        explicit Module(std::string_view str, const FunctionCallbackFn& func = nullptr);

        // get rwx segments of a module
        std::vector<Segments>& get_segments()
        {
            return _segments;
        }

        // get base address of a module
        [[nodiscard]] std::uintptr_t base() const
        {
            return _baseAddress;
        }

        void* get_proc(std::string_view proc_name);
    };
}
