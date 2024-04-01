#ifndef MCPPROTOCOL_DESERIALIZER_HPP
#define MCPPROTOCOL_DESERIALIZER_HPP

#include <cstdint>
#include <vector>

namespace mcp {
    template <typename PacketT>
    [[nodiscard]] std::vector<std::byte> deserialize([[maybe_unused]] PacketT packet) {
//        auto buffer = std::vector<std::byte>();
    }
} // mcp

#endif //MCPPROTOCOL_DESERIALIZER_HPP
