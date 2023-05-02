#include <gensokyo.hpp>
#include <tlhelp32.h>

gensokyo::WinProcess::WinProcess(std::string_view name, AttachType type)
{
    if (type == AttachType::ProcessName)
    {
        if (!WinProcess::attach_by_process_name(name))
            throw std::runtime_error("Failed to attach to process");
    }
    else if (type == AttachType::WindowName)
    {
        if (!attach_by_window_name(name, ""))
            throw std::runtime_error("Failed to attach to process");
    }
    else if (type == AttachType::WindowClassName)
    {
        if (!attach_by_window_name("", name))
            throw std::runtime_error("Failed to attach to process");
    }
    else
        throw std::runtime_error("What the fuck");
}

gensokyo::WinProcess::~WinProcess()
{
    CloseHandle(_handle);
}

bool gensokyo::WinProcess::read_impl(const std::uintptr_t address, void* buffer, const std::size_t size)
{
    std::size_t read_bytes {};
    ReadProcessMemory(_handle, reinterpret_cast<void*>(address), buffer, size, &read_bytes);
    return read_bytes == size;
}

bool gensokyo::WinProcess::write_impl(const std::uintptr_t address, void* buffer, const std::size_t size)
{
    std::size_t written_bytes {};
    WriteProcessMemory(_handle, reinterpret_cast<void*>(address), buffer, size, &written_bytes);
    return written_bytes == size;
}

bool gensokyo::WinProcess::attached()
{
    return _pid != 0 && _handle != nullptr;
}

bool gensokyo::WinProcess::attach_by_process_name(std::string_view process_name)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    for (auto ok = Process32First(snapshot, &entry); ok; ok = Process32Next(snapshot, &entry))
    {
        if (process_name == entry.szExeFile)
        {
            const auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
            [[unlikely]] if (!handle)
            {
                CloseHandle(snapshot);
                return false;
            }

            _pid    = static_cast<std::uint32_t>(entry.th32ProcessID);
            _handle = handle;
            CloseHandle(snapshot);
            return true;
        }
    }

    CloseHandle(snapshot);
    return false;
}

bool gensokyo::WinProcess::attach_by_window_name(std::string_view window_name, std::string_view window_class_name)
{
    const auto hwnd = FindWindowA(window_name.empty() ? nullptr : window_name.data(), window_class_name.empty() ? nullptr : window_class_name.data());
    if (!hwnd)
        return false;

    GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&_pid));
    _handle = OpenProcess(PROCESS_ALL_ACCESS, false, _pid);

    if (!_handle)
    {
        CloseHandle(_handle);
        return false;
    }

    return true;
}