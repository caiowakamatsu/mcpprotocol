#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "packet_frame.hpp"

namespace mcp {


    template <mcp::version version>
    struct protocol {
        static std::vector<std::byte> serialize(mcp::outbound_packet_frame frame) {
            // this would eventually take "state" (to be able to do compression / encryption)
            auto data = std::vector<std::byte>();
            auto writer = mcp::writer(data);
            writer.write(mcp::var_int(frame.id));
            writer.write(mcp::var_int(frame.data.size()));
            writer.write(frame.data);

            return data;
        }
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
