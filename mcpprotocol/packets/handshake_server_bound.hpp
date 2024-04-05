#ifndef MCPPROTOCOL_HANDSHAKE_SERVER_BOUND_HPP
#define MCPPROTOCOL_HANDSHAKE_SERVER_BOUND_HPP

#include <mcpprotocol/packet_base.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/writer.hpp>

#include <string>

namespace mcp {
    template<auto Handler>
    struct handshake_s : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                auto *state,
                std::int32_t protocol_version,
                std::string server_name,
                std::uint16_t server_port,
                std::uint8_t next_state) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(protocol_version));
            writer.write(server_name);
            writer.write(server_port);
            writer.write(mcp::var_int(next_state));
            state->mode = next_state == 1 ? stream_mode::status : stream_mode::login;

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);
            const auto protocol_version = reader.read<mcp::var_int>().value;
            const auto server_name = reader.read<std::string>();
            const auto server_port = reader.read<std::uint16_t>();
            const auto next_state = reader.read<mcp::var_int>().value;
            state->mode = next_state == 1 ? stream_mode::status : stream_mode::login;
            handshake_s::handler(base_handle, protocol_version, server_name, server_port, next_state);
        }
    };

    template<auto Handler>
    struct legacy_ping_s : public detail::packet_base<0xFE, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t payload) {
            auto buffer = std::vector<std::byte>();
            // I'm going to go ahead and break convention here...
            buffer.push_back(std::bit_cast<std::byte>(payload));
            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
            // and I'll break it here too! Should be fine...
            legacy_ping_s::handler(base_handle, source[0]);
        }
    };
}

#endif //MCPPROTOCOL_HANDSHAKE_SERVER_BOUND_HPP
