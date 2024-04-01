#ifndef MCPPROTOCOL_PACKET_FRAME_HPP
#define MCPPROTOCOL_PACKET_FRAME_HPP

#include <type_traits>
#include <concepts>

namespace mcp {
    template <typename T>
    concept is_packet = requires (T t, mcp::writer w, mcp::reader r) {
        { t.write(w) } -> std::same_as<void>;
        { t.read(r) } -> std::same_as<void>;
    };

    // non-compressed { var_int length, var_int id, byte[] data }
    // compressed { var_int length, var_int data_length, {var_int packet_id, byte[] data} }

    struct packet_frame {
        std::uint32_t length;
    };
}

#endif //MCPPROTOCOL_PACKET_FRAME_HPP
