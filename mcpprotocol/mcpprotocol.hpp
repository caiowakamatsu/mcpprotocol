#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "deserializer.hpp"
#include "packet_frame.hpp"
#include "network_state.hpp"

namespace mcp {
    template <mcp::version version>
    struct protocol {
        template <template <auto Handler> typename Packet>
        std::vector<std::byte> serialize() {

        }
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
