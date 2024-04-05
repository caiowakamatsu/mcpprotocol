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
