#ifndef MCPPROTOCOL_READER_HPP
#define MCPPROTOCOL_READER_HPP

#include <array>
#include <bit>
#include <cstddef>
#include <concepts>
#include <span>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <bitset>

#include <mcpprotocol/types.hpp>

namespace mcp {

    class reader {
    public:
        explicit reader(std::span<const std::byte> buffer, std::uint64_t cursor = 0);

        std::span<const std::byte> read_n(std::uint64_t count);

        template <typename T>
        [[nodiscard]] T read() = delete;

        template <typename T>
        requires std::floating_point<T> || std::integral<T>
        [[nodiscard]] T read() {
            if constexpr (sizeof(T) == 1) {
                return std::bit_cast<T>(read_n(1)[0]);
            } else {
                const auto bytes = read_n(sizeof(T));
                auto as_array = std::array<std::byte, sizeof(T)>();
                std::memcpy(as_array.data(), bytes.data(), sizeof(T));
                std::reverse(as_array.begin(), as_array.end());
                auto value = T();
                std::memcpy(&value, as_array.data(), sizeof(T));
                return value;
            }
        }

        template <size_t BitCount>
        [[nodiscard]] std::bitset<BitCount> read_bitset(bool fixed = false) {
            constexpr auto bytes_required = static_cast<std::uint32_t>(std::ceil((float(BitCount) / 8.0f)));

            auto value = std::bitset<BitCount>();
            if (fixed) {
                const auto bytes = read_n(bytes_required);
                for (std::size_t i = 0; i < bytes_required; ++i) {
                    for (std::size_t j = 0; j < 8; ++j) {
                        if (i * 8 + j < BitCount) {
                            value.set(i * 8 + j, bytes[i] & std::byte(0x01 << j));
                        }
                    }
                }
            } else {
                constexpr auto longs_required = static_cast<std::uint32_t>(std::ceil(
                        float(bytes_required) / float(sizeof(std::uint64_t))));
                for (std::size_t i = 0; i < longs_required; ++i) {
                    auto as_long = std::uint64_t();
                    std::memcpy(&as_long, read_n(8).data(), 8);
                    value |= std::bitset<BitCount>(as_long);
                    for (std::size_t j = 0; j < 64; ++j) {
                        if (i * 64 + j < BitCount) {
                            value.set(i * 64 + j, as_long & std::uint64_t(0x01 << j));
                        }
                    }
                }
            }

            return value;
        }

        [[nodiscard]] std::span<const std::byte> remaining() const;

    private:
        void ensure_remaining(std::uint64_t count);

        std::span<const std::byte> buffer;
        std::uint64_t cursor;
    };

    template <>
    [[nodiscard]] inline bool reader::read<bool>() {
        return static_cast<bool>(read<std::uint8_t>());
    }

    template <>
    [[nodiscard]] mcp::var_int reader::read<mcp::var_int>();

    template <>
    [[nodiscard]] mcp::var_long reader::read<mcp::var_long>();

    template <>
    [[nodiscard]] inline std::string reader::read<std::string>() {
        auto str = std::string();
        const auto length = read<var_int>().value;
        const auto data = read_n(length);
        return { reinterpret_cast<const char*>(data.data()), static_cast<std::size_t>(length) };
    }
} // mcp

#endif //MCPPROTOCOL_READER_HPP
