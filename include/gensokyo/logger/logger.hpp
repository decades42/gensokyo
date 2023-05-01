#pragma once

// Maybe use spdlog for logging
#ifdef WINDOWS
    #include <Windows.h>
#endif
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <memory>

namespace gensokyo::impl
{
    class Logger
    {
        [[nodiscard]] std::string format_time() const
        {
            const auto& t = std::time(nullptr);
            return fmt::format("[{:%H:%M:%S}]", fmt::localtime(t));
        }

#ifdef WINDOWS
        bool _initialized {};
#endif

      public:
        Logger()                         = default;
        Logger(const Logger&)            = default;
        Logger(Logger&&)                 = default;
        Logger& operator=(const Logger&) = default;
        Logger& operator=(Logger&&)      = default;

#ifdef WINDOWS
        bool setup()
        {
            _initialized = AllocConsole();
            if (!_initialized)
                return false;

            freopen_s(reinterpret_cast<_iobuf**>(stdout), "CONOUT$", "w", stdout);
            SetConsoleTitleA("gensokyo");

            // enable color support
            auto std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            // its ok if teh handle is valid
            if (std_handle == INVALID_HANDLE_VALUE)
                return true;

            DWORD mode;
            if (!GetConsoleMode(std_handle, &mode))
                return true;

            // already enabled
            if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                return true;

            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(std_handle, mode);

            return true;
        }

        void destroy() const
        {
            if (!_initialized)
                return;
            fclose(stdin);
            fclose(stdout);
            fclose(stderr);
            FreeConsole();
        }
#endif

        template <typename... Args>
        auto info(fmt::format_string<Args...> fmt, Args&&... args)
        {
            fmt::println("{} [-] {}", format_time(), fmt::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        auto error(fmt::format_string<Args...> fmt, Args&&... args)
        {
            fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "{} [x] {}\n", format_time(), fmt::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        auto success(fmt::format_string<Args...> fmt, Args&&... args)
        {
            fmt::print(fmt::emphasis::bold | fg(fmt::color::light_green), "{} [+] {}\n", format_time(), fmt::format(fmt, std::forward<Args>(args)...));
        }
    };
}

namespace gensokyo
{
    inline std::unique_ptr<impl::Logger> logger {};
}