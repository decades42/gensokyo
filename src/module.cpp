#include <gensokyo.hpp>

#ifdef WINDOWS
    #include <minwindef.h>
    #include <Windows.h>
#endif

#include <stdexcept>
#include <string>

gensokyo::impl::Module::Module(const std::string& str)
{
    get_module_nfo(str);
}

void gensokyo::impl::Module::get_module_nfo(std::string_view str)
{
#ifdef WINDOWS
    const auto handle = GetModuleHandleA(str.empty() ? nullptr : str.data());
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

    if (!logger)
        return;

    logger->success("{} | base_addr:{:#05x} | size:{:#05x} | _segments.size():{}", str, _baseAddress, _size, _segments.size());
#endif
}

void* gensokyo::impl::Module::get_proc(std::string_view proc_name)
{
#ifdef WINDOWS
    if (!this->_handle)
        throw std::runtime_error("Invalid module handle  when getting ProcAddress");

    if (const auto address = GetProcAddress(static_cast<HMODULE>(_handle), proc_name.data()); address)
    {
        return address;
    }
#endif
    return nullptr;
}
