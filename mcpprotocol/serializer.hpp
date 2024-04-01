#ifndef MCPPROTOCOL_SERIALIZER_HPP
#define MCPPROTOCOL_SERIALIZER_HPP

#include <cstdint>
#include <vector>

#include "writer.hpp"

namespace mcp {
    template <typename PacketT>
    [[nodiscard]] std::vector<std::byte> serialize(PacketT packet) {
        auto buffer = std::vector<std::byte>();
        [[maybe_unused]] auto writer = mcp::writer(buffer);
        packet.write(writer);
        return buffer;
    }
} // mcp

#endif //MCPPROTOCOL_SERIALIZER_HPP
