#ifndef MCPPROTOCOL_WRITER_HPP
#define MCPPROTOCOL_WRITER_HPP

#include <array>
#include <cstddef>
#include <concepts>
#include <span>
#include <vector>

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

    private:

        std::vector<std::byte> * const vector = nullptr;
    };

} // mcp

#endif //MCPPROTOCOL_WRITER_HPP
