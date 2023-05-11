#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace gensokyo::impl
{
    class Process
    {
      public:
        bool read(std::uintptr_t address, void* buffer, std::size_t size);
        bool write(std::uintptr_t address, void* buffer, std::size_t size);

        template <typename T>
        std::optional<T> read(std::uintptr_t address)
        {
            T res {};
            if (!read(address, &res, sizeof(T)))
                return std::nullopt;

            return res;
        }

        template <typename T>
        bool write(std::uintptr_t address, T value)
        {
            return write(address, &value, sizeof(T));
        }

        virtual std::uint32_t get_pid()
        {
            return 0;
        }

        virtual bool attached()
        {
            return false;
        }

      protected:
        virtual bool read_impl([[maybe_unused]] std::uintptr_t address, [[maybe_unused]] void* buffer, [[maybe_unused]] std::size_t size)
        {
            return true;
        }

        virtual bool write_impl([[maybe_unused]] std::uintptr_t address, [[maybe_unused]] void* buffer, [[maybe_unused]] std::size_t size)
        {
            return true;
        }

        virtual bool attach_by_process_name([[maybe_unused]] std::string_view process_name)
        {
            return true;
        }
    };
}