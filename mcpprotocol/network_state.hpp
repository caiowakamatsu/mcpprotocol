#ifndef MCPPROTOCOL_NETWORK_STATE_HPP
#define MCPPROTOCOL_NETWORK_STATE_HPP

#include <optional>

namespace mcp {
    enum class stream_mode {
        handshaking,
        status,
        login,
        configuration,
        play,
    };

    // There might be some way to not have to duplicate definitions
    // for no user data. As it stands - I don't care enough to figure
    // it out, so if someone wants to fix it up :) ty.
    template <typename UserData>
    struct basic_network_state {
        std::optional<std::uint32_t> compression_threshold = std::nullopt;
        stream_mode mode = stream_mode::handshaking;
        std::vector<std::byte> buffer;
        UserData user_data;
    };

    template <>
    struct basic_network_state<void> {
        std::optional<std::uint32_t> compression_threshold = std::nullopt;
        stream_mode mode = stream_mode::handshaking;
        std::vector<std::byte> buffer;
    };
}

#endif //MCPPROTOCOL_NETWORK_STATE_HPP
