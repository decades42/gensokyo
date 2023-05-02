#include <gensokyo.hpp>

bool gensokyo::impl::Process::read(std::uintptr_t address, void* buffer, std::size_t size)
{
    return read_impl(address, buffer, size);
}

bool gensokyo::impl::Process::write(std::uintptr_t address, void* buffer, std::size_t size)
{
    return write_impl(address, buffer, size);
}
