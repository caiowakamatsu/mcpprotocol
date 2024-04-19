#ifndef MCPPROTOCOL_LOGIN_SERVER_BOUND_HPP
#define MCPPROTOCOL_LOGIN_SERVER_BOUND_HPP

#include <mcpprotocol/types.hpp>
#include <mcpprotocol/packet_base.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/writer.hpp>

namespace mcp {
    template<auto Handler>
    struct login_start_s : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string player_name,
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(player_name);
            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto player_name = reader.read<std::string>();
            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);

            login_start_s::handler(base_handle, player_name, uuid);
        }
    };

    template<auto Handler>
    struct login_encryption_response_s : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<std::byte> shared_secret,
                std::span<std::byte> verify_token) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(shared_secret.size()));
            writer.write(shared_secret);
            writer.write(mcp::var_int(verify_token.size()));
            writer.write(verify_token);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto shared_secret_length = reader.read<mcp::var_int>();
            const auto shared_secret = reader.read_n(shared_secret_length.value);
            const auto verify_token_length = reader.read<mcp::var_int>();
            const auto verify_token = reader.read_n(verify_token_length.value);

            login_encryption_response_s::handler(base_handle, shared_secret, verify_token);
        }
    };

    template<auto Handler>
    struct login_plugin_response_s : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int message_id,
                bool success,
                std::span<std::byte> data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(message_id);
            writer.write(success);
            writer.write(data);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message_id = reader.read<mcp::var_int>();
            const auto success = reader.read<bool>();
            const auto data = reader.remaining();

            login_plugin_response_s::handler(base_handle, message_id, success, data);
        }
    };

    template<auto Handler>
    struct login_login_acknowledged : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            login_login_acknowledged::handler(base_handle);
        }
    };
}

#endif //MCPPROTOCOL_LOGIN_SERVER_BOUND_HPP
