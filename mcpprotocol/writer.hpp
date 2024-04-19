#ifndef MCPPROTOCOL_WRITER_HPP
#define MCPPROTOCOL_WRITER_HPP

#include <array>
#include <cstddef>
#include <concepts>
#include <span>
#include <vector>
#include <string_view>
#include <algorithm>
#include <cstring>
#include <bitset>
#include <cmath>

#include <mcpprotocol/types.hpp>

namespace mcp {

    class writer {
    public:
        explicit writer(std::vector<std::byte> &vector);

        void write(std::byte byte);

        void write(std::span<const std::byte> bytes);

        template <typename T>
            requires std::floating_point<T> || std::integral<T>
        void write(T value) {
            auto bytes = std::array<std::byte, sizeof(T)>();
            std::memcpy(bytes.data(), &value, sizeof(T));
            std::reverse(bytes.begin(), bytes.end());
            write(bytes);
        }

        void write(bool value);

        void write(std::string_view value);

        void write(mcp::var_int value);

        void write(mcp::var_long value);

        template <size_t BitCount>
        void write(std::bitset<BitCount> value, bool fixed = false) {
            constexpr auto bytes_required = static_cast<std::uint32_t>(std::ceil((float(BitCount) / 8.0f)));
            auto bytes = std::array<std::byte, bytes_required>();
            for (std::size_t i = 0; i < bytes_required; ++i) {
                for (std::size_t j = 0; j < 8; ++j) {
                    bytes[i] |= value.test(i * 8 + j) ? (0x01 << j) : 0;
                }
            }
            if (fixed) {
                write(bytes);
            } else {
                constexpr auto longs_required = static_cast<std::uint32_t>(std::ceil(
                        float(bytes_required) / float(sizeof(std::uint64_t))));
                for (std::size_t i = 0; i < longs_required; ++i) {
                    auto as_long = std::uint64_t();
                    std::memcpy(&as_long, bytes.data() + i * 8, 8);
                    write(as_long);
                }
            }
        }
    private:

        std::vector<std::byte> * const vector = nullptr;
    };

} // mcp

#endif //MCPPROTOCOL_WRITER_HPP
