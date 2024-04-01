#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"

namespace mcp {
    template <mcp::version version>
    struct versioned_protocol {
        template <typename T>
        static std::vector<std::byte> serialize(T packet) {
            return mcp::serialize(packet);
        }
//        template <typename T>
//        static void deserializer(T packet) {
//            deserialize(packet);
//        }
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
