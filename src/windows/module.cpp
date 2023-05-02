#include <gensokyo.hpp>

#include <minwindef.h>
#include <Windows.h>
#include <stdexcept>

gensokyo::impl::Module::Module(const std::string_view str, const FunctionCallbackFn& func)
{
    get_module_nfo(str, func);
}

void gensokyo::impl::Module::get_module_nfo(std::string_view mod, const FunctionCallbackFn& func)
{
    const auto handle = GetModuleHandleA(mod.empty() ? nullptr : mod.data());
    if (!handle)
        throw std::runtime_error("Failed to get module handle");

    const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        throw std::runtime_error("Invalid dos magic.");

    const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
        throw std::runtime_error("Invalid nt signature");

    this->_handle      = handle;
    this->_baseAddress = reinterpret_cast<uintptr_t>(handle);
    this->_size        = ntHeader->OptionalHeader.SizeOfImage;
    auto section       = IMAGE_FIRST_SECTION(ntHeader);

    for (auto i = 0; i < ntHeader->FileHeader.NumberOfSections; i++, section++)
    {
        const auto is_executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;

        if (const auto is_readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0; is_executable && is_readable)
        {
            const auto start = reinterpret_cast<uintptr_t>(handle) + section->VirtualAddress;
            const auto size  = std::min(section->SizeOfRawData, section->Misc.VirtualSize);

            this->_segments.emplace_back(start, reinterpret_cast<std::uint8_t*>(start), size);
        }
    }

    logger.success("{} | base_addr:{:#05x} | size:{:#05x} | _segments.size():{}", mod, _baseAddress, _size, _segments.size());

    if (func)
    {
        const auto start = reinterpret_cast<uint8_t*>(handle);
        const std::vector data(start, start + _size);
        func(data);
    }
}

void* gensokyo::impl::Module::get_proc(const std::string_view proc_name)
{
    if (!this->_handle)
        throw std::runtime_error("Invalid module handle when getting ProcAddress");

    if (const auto address = GetProcAddress(static_cast<HMODULE>(_handle), proc_name.data()); address)
    {
        return address;
    }
    throw std::runtime_error(fmt::format("Cannot get proc with name {}", proc_name));
}
