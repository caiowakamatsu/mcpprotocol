#ifndef MCPPROTOCOL_LOGIN_CLIENT_BOUND_HPP
#define MCPPROTOCOL_LOGIN_CLIENT_BOUND_HPP

#include <mcpprotocol/types.hpp>
#include <mcpprotocol/packet_base.hpp>

namespace mcp {
    template<auto Handler>
    struct login_disconnect_c : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string reason) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(reason);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto reason = reader.read<std::string>();

            login_disconnect_c::handler(base_handle, reason);
        }
    };

    template<auto Handler>
    struct encryption_request_c : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string server_id,
                std::span<std::byte> public_key,
                std::span<std::byte> verify_token) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(server_id);
            writer.write(mcp::var_int(public_key.size()));
            writer.write(public_key);
            writer.write(mcp::var_int(verify_token.size()));
            writer.write(verify_token);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto server_id = reader.read<std::string>();
            auto public_key_length = reader.read<mcp::var_int>();
            auto public_key = reader.read_n(public_key_length.value);
            auto verify_token_length = reader.read<mcp::var_int>();
            auto verify_token = reader.read_n(verify_token_length.value);

            encryption_request_c::handler(base_handle, server_id, public_key, verify_token);
        }
    };

    struct login_success_property {
        std::string name;
        std::string value;
        std::optional<std::string> signature;
    };
    template<auto Handler>
    struct login_success_c : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                std::string username,
                std::span<login_success_property> properties) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(username);
            writer.write(mcp::var_int(properties.size()));
            for (const auto &property : properties) {
                writer.write(property.name);
                writer.write(property.value);
                writer.write(property.signature.has_value());
                if (property.signature.has_value()) {
                    writer.write(property.signature.value());
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            auto username = reader.read<std::string>();
            auto properties_count = reader.read<mcp::var_int>();

            std::vector<typename detail::get_type_t<mcp::login_success_property, Converters...>::type_target> properties;
            properties.reserve(properties_count.value);

            for (std::size_t i = 0; i < properties_count.value; ++i) {
                properties.push_back(detail::get_type_t<mcp::login_success_property, Converters...>::from(reader));
            }

            login_success_c::handler(base_handle, uuid, username, properties);
        }
    };

    template<auto Handler>
    struct set_compression_c : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int threshold) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(threshold);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto threshold = reader.read<mcp::var_int>();

            set_compression_c::handler(base_handle, threshold);
        }
    };

    template<auto Handler>
    struct login_plugin_request_c : public detail::packet_base<0x04, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int message_id,
                std::string identifier,
                std::span<std::byte> data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(message_id);
            writer.write(identifier);
            writer.write(data);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message_id = reader.read<mcp::var_int>();
            const auto identifier = reader.read<std::string>();
            const auto data = reader.remaining(); // Is this correct?

            login_plugin_request_c::handler(base_handle, message_id, identifier, data);
        }
    };
}

#endif //MCPPROTOCOL_LOGIN_CLIENT_BOUND_HPP
