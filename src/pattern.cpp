#include <gensokyo.hpp>
#include <bitset>

gensokyo::Address gensokyo::pattern::impl::find_brute_force(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern) noexcept
{
    const auto pattern_size = pattern.size();
    const auto end          = data.end() - pattern_size;

    for (auto current = data.begin(); current <= end; ++current)
    {
        bool found = true;

        for (std::size_t j = 0; j < pattern_size; ++j)
        {
            if (const auto& pattern_byte = pattern[j]; pattern_byte && *pattern_byte != *(current + j))
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            return &*current;
        }
    }
    return {};
}

// https://github.com/BasedInc/libhat
gensokyo::Address gensokyo::pattern::impl::find_std(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern) noexcept
{
    const auto pattern_size = pattern.size();
    const auto end          = data.end() - pattern_size;
    const auto first_byte   = pattern[0].value();

    for (auto current = data.begin(); current <= end; ++current)
    {
        current = std::find(current, end, first_byte);

        if (current == end)
        {
            break;
        }

        auto matched = std::equal(pattern.begin() + 1,
                                  pattern.end(),
                                  current + 1,
                                  [](auto opt, auto byte)
                                  {
                                      return !opt.has_value() || *opt == byte;
                                  });

        if (matched)
        {
            return &*current;
        }
    }
    return {};
}

template <typename SIMD>
gensokyo::Address gensokyo::pattern::impl::find_simd(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern) noexcept
{
    constexpr int simd_length = SIMD::simd_length;
    const auto pattern_size   = pattern.size();

    // when pattern size is larger than the current cpu instruction supports, fallback to std implementation
    if (pattern_size > simd_length)
        return find_std(data, pattern);

    auto make_pattern_simd = [&]
    {
        std::array<std::byte, simd_length> bytes {};
        std::array<std::byte, simd_length> masks {};
        for (std::size_t i = 1; i < pattern_size; i++)
        {
            if (pattern[i].has_value())
            {
                bytes[i - 1] = std::byte { pattern[i].value() };
                masks[i - 1] = std::byte { 0xff };
            }
        }

        return std::make_pair(SIMD::load_unaligned(bytes.data()), SIMD::load_unaligned(masks.data()));
    };

    const uint8_t* current = data.data();
    const uint8_t* end     = current + data.size();

    // pattern data
    const auto first_byte                     = SIMD::set1_epi8(static_cast<int8_t>(pattern[0].value()));
    const auto [pattern_bytes, pattern_masks] = make_pattern_simd();

    // load dataset into simd
    auto simd_data_ptr        = SIMD::cast(data.data());
    const auto num_iterations = static_cast<size_t>(end - pattern_size - data.data()) / simd_length;
    auto simd_end_ptr         = simd_data_ptr + num_iterations;

    for (; simd_data_ptr != simd_end_ptr; ++simd_data_ptr)
    {
        const auto cmp = SIMD::cmpeq_epi8(first_byte, SIMD::load_unaligned(simd_data_ptr));
        auto mask      = static_cast<uint32_t>(SIMD::movemask_epi8(cmp));
        while (mask)
        {
            // Find the index of the least significant set bit (first match)
            std::uint32_t offset;
            if constexpr (simd_length == 32)
            {
                offset = _tzcnt_u32(mask);
            }
            else
            {
#ifdef MSVC
                unsigned long temp;
                _BitScanForward(&temp, mask);
                offset = temp;
#elif defined(GCC) || defined(CLANG)
                offset = __builtin_ctz(mask);
#endif
            }

            // Calculate the pointer to the matched byte in the data
            const auto byte_ptr = reinterpret_cast<const std::uint8_t*>(simd_data_ptr) + offset;

            // Load the data chunk after the matched first byte into a SIMD register
            const auto data_chunk = SIMD::load_unaligned(SIMD::cast(byte_ptr + 1));

            // Compare the data chunk to the pattern bytes (excluding the first byte)
            const auto cmp_to_sig = SIMD::cmpeq_epi8(pattern_bytes, data_chunk);

            // Test if all the required bytes in the pattern match the data chunk
            const auto matched = SIMD::test(cmp_to_sig, pattern_masks);

            // If the entire pattern is matched, return the address of the match
            if (matched)
            {
                return byte_ptr;
            }

            // Clear the least significant set bit and continue searching for matches
            if constexpr (simd_length == 32)
                mask = _blsr_u32(mask);
            else
                mask &= (mask - 1);
        }
    }

    // Look in remaining bytes that couldn't be grouped into simd_length * 8 bits

    const auto remaining_size = reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(simd_data_ptr);
    return find_std(std::span(reinterpret_cast<std::uint8_t*>(simd_data_ptr), remaining_size), pattern);
}

gensokyo::Address gensokyo::pattern::find(const std::span<std::uint8_t>& data, const std::span<impl::HexData>& pattern) noexcept
{
    const auto arch = cpu.get_arch();

    if (arch == CPUArch::AVX2 || arch == CPUArch::SSE)
    {
        const auto pattern_size = pattern.size();
        if (pattern_size <= 33 && arch == CPUArch::AVX2)
            return impl::find_simd<simd::iAVX2>(data, pattern);
        if (pattern_size <= 17)
            return impl::find_simd<simd::iSSE>(data, pattern);
    }

    return impl::find_std(data, pattern);
}

gensokyo::Address gensokyo::pattern::find(const std::span<std::uint8_t>& data, impl::Pattern<> pattern) noexcept
{
    return find(data, pattern.bytes);
}
