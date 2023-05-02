#include <gensokyo.hpp>

int main()
{
    gensokyo::logger.setup();

    try
    {
        gensokyo::WinProcess process("notepad.exe", gensokyo::WinProcess::AttachType::ProcessName);

        if (!process.attached())
        {
            gensokyo::logger.error("Faield to attach");
            return 1;
        }

        gensokyo::logger.success("process attached");

    }
    catch (std::runtime_error& ex)
    {
        gensokyo::logger.error("Exception: {}", ex.what());
        return 1;
    }

    return 0;
}