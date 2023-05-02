#pragma once

#include "address.hpp"
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <vector>
#include <array>
#include <ranges>

namespace gensokyo::pattern
{
    namespace impl
    {
        using HexData = std::optional<std::uint8_t>;

        [[nodiscard]] static constexpr std::optional<uint8_t> hex_char_to_byte(char c)
        {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;

            return std::nullopt;
        }

        template <char Wildcard = '?'>
        [[nodiscard]] static constexpr std::optional<std::uint8_t> parse_hex(std::string_view str)
        {
            if (str.size() == 1 && str.front() == Wildcard)
                return std::nullopt;

            if (str.size() != 2)
                return std::nullopt;

            auto high = hex_char_to_byte(str[0]);
            auto low  = hex_char_to_byte(str[1]);

            // if both are not wildcard, e.g AA
            if (high.has_value() && low.has_value())
            {
                return (high.value() << 4) | low.value();
            }

            return std::nullopt;
            /*
            comment these out when find pattern supports this kind of pattern
            // A?
            else if (high.has_value() && !low.has_value())
            {
                return high.value() << 4;
            }
            // ?A
            else if (!high.has_value() && low.has_value())
            {
                return low.value();
            }
            */
        }

        template <char Delimiter = ' ', char Wildcard = '?'>
        [[nodiscard]] static constexpr std::vector<HexData> parse_pattern(std::string_view pattern)
        {
            std::vector<HexData> result {};
            for (const auto& str : pattern | std::views::split(Delimiter))
            {
                const std::string_view token(str);
                result.push_back(parse_hex<Wildcard>(token));
            }
            return result;
        }

        // https://stackoverflow.com/a/73014828
        template <auto N>
        static constexpr auto str(char const (&cstr)[N])
        {
            std::array<char, N> arr;
            for (std::size_t i = 0; i < N; ++i)
                arr[i] = cstr[i];
            return arr;
        }

        template <auto str>
        inline constexpr auto make_pattern()
        {
            const auto sig      = impl::parse_pattern(str.data());
            constexpr auto size = impl::parse_pattern(str.data()).size();
            std::array<impl::HexData, size> arr {};
            for (std::size_t i = 0; i < size; i++)
                arr[i] = sig[i];
            return arr;
        }

        template <char Delimiter = ' ', char Wildcard = '?'>
        struct Pattern
        {
            constexpr Pattern(std::string_view pattern)
            {
                bytes = parse_pattern<Delimiter, Wildcard>(pattern);
                if (!bytes.front().has_value())
                    throw std::invalid_argument("A pattern shouldn't start with a wildcard!!");
            }

            constexpr std::size_t size() const
            {
                return bytes.size();
            }

            constexpr const HexData& operator[](std::size_t index) const
            {
                return bytes[index];
            }

            std::vector<HexData> bytes;
        };

        gensokyo::Address find_brute_force(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern);
        gensokyo::Address find_std(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern);

        template <typename SIMD>
        gensokyo::Address find_simd(const std::span<std::uint8_t>& data, const std::span<HexData>& pattern);
    }

    gensokyo::Address find(const std::span<std::uint8_t>& data, impl::Pattern<> pattern);
    gensokyo::Address find(const std::span<std::uint8_t>& data, const std::span<impl::HexData>& pattern);

    using Type = impl::Pattern<' ', '?'>;
}

#define GENSOKYO_MAKE_PATTERN(p) gensokyo::pattern::impl::make_pattern<gensokyo::pattern::impl::str(p)>();