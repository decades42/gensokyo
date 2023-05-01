#include <gensokyo.hpp>
#include <iostream>
#include <unordered_map>

std::unordered_map<int, std::string> arch_name = {
    {0, "None"},
    {1, "SSE2"},
    {2, "AVX2"}
};

int main()
{
    std::cout << arch_name[static_cast<int>(gensokyo::cpu.get_arch())] << std::endl;
}