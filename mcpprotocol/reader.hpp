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
                auto value = T();
                for (size_t i = 0; i < sizeof(T); i++) {
                    std::memcpy(&value + i, bytes.data() + (sizeof(T) - i - 1), 1);
                }
                return value;
            }
        }


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
    [[nodiscard]] inline std::string reader::read<std::string>() {
        auto str = std::string();
        const auto length = read<std::uint32_t>();
        const auto data = read_n(length);
        return { reinterpret_cast<const char*>(data.data()), length };
    }

} // mcp

#endif //MCPPROTOCOL_READER_HPP
