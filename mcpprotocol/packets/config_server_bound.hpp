#ifndef MCPPROTOCOL_CONFIG_SERVER_BOUND_HPP
#define MCPPROTOCOL_CONFIG_SERVER_BOUND_HPP

namespace mcp {
    template<auto Handler>
    struct config_client_information_s : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string locale,
                std::int8_t view_distance,
                mcp::var_int chat_mode,
                bool chat_colors,
                std::uint8_t displayed_skin_parts,
                mcp::var_int main_hand,
                bool enable_text_filtering,
                bool allow_server_listing) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(locale);
            writer.write(view_distance);
            writer.write(chat_mode);
            writer.write(chat_colors);
            writer.write(displayed_skin_parts);
            writer.write(main_hand);
            writer.write(enable_text_filtering);
            writer.write(allow_server_listing);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto locale = reader.read<std::string>();
            const auto view_distance = reader.read<std::int8_t>();
            const auto chat_mode = reader.read<mcp::var_int>();
            const auto chat_colors = reader.read<bool>();
            const auto displayed_skin_parts = reader.read<std::uint8_t>();
            const auto main_hand = reader.read<mcp::var_int>();
            const auto enable_text_filtering = reader.read<bool>();
            const auto allow_server_listing = reader.read<bool>();

            config_client_information_s::handler(base_handle,
                                                 locale,
                                                 view_distance,
                                                 chat_mode,
                                                 chat_colors,
                                                 displayed_skin_parts,
                                                 main_hand,
                                                 enable_text_filtering,
                                                 allow_server_listing);
        }
    };

    template<auto Handler>
    struct config_plugin_message_s : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string channel,
                std::span<std::byte> data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(channel);
            writer.write(data);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto channel = reader.read<std::string>();
            const auto data = reader.remaining();

            config_plugin_message_s::handler(base_handle, channel, data);
        }
    };

    template<auto Handler>
    struct config_acknowledge_finish_configuration_s : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            config_acknowledge_finish_configuration_s::handler(base_handle);
        }
    };

    template<auto Handler>
    struct config_keep_alive_s : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t keep_alive_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(keep_alive_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto keep_alive_id = reader.read<std::int64_t>();

            config_keep_alive_s::handler(base_handle, keep_alive_id);
        }
    };

    template<auto Handler>
    struct config_pong_s : public detail::packet_base<0x04, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto id = reader.read<std::int32_t>();

            config_pong_s::handler(base_handle, id);
        }
    };

    template<auto Handler>
    struct config_resource_pack_response_s : public detail::packet_base<0x05, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                mcp::var_int result) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(result);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto result = reader.read<mcp::var_int>();

            config_resource_pack_response_s::handler(base_handle, uuid, result);
        }
    };


}

#endif //MCPPROTOCOL_CONFIG_SERVER_BOUND_HPP
