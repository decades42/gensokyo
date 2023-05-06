#pragma once

// #include "../process.hpp"

namespace gensokyo
{
    class WinProcess : public impl::Process
    {
      public:
        enum class AttachType
        {
            ProcessName,
            WindowName,
            WindowClassName,
        };

      private:
        HANDLE _handle {};
        std::uint32_t _pid {};

      public:
        /*
         * @param name This depends on AttachType, when AttachType is ProcessName then it should be process name, like "notepad.exe" and so on
         * @param type What method should use to attach to a process
         *
         * Will throw an runtime_error exception when it fails to attach to process
         */
        WinProcess(std::string_view name, AttachType type);
        ~WinProcess();

        std::uint32_t get_pid() override
        {
            return _pid;
        }

        bool attached() override;

      protected:
        bool read_impl(std::uintptr_t address, void* buffer, std::size_t size) override;
        bool write_impl(std::uintptr_t address, void* buffer, std::size_t size) override;
        bool attach_by_process_name(std::string_view process_name) override;

      private:
        bool attach_by_window_name(std::string_view window_name, std::string_view window_class_name);
    };
}