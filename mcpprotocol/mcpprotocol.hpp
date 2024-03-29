#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"

namespace mcp {
    template <mcp::version version>
    struct versioned_protocol {
        using serializer = mcp::serializer;
        using deserializer = mcp::deserializer;
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
