#ifndef MCPPROTOCOL_PACKET_FRAME_HPP
#define MCPPROTOCOL_PACKET_FRAME_HPP

#include <type_traits>
#include <concepts>
#include <mcpprotocol/packet_traits.hpp>

namespace mcp {
    struct packet_frame {
        std::uint32_t length;
    };
}

#endif //MCPPROTOCOL_PACKET_FRAME_HPP
