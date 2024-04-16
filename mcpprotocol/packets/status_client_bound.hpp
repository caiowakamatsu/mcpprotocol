#ifndef MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP
#define MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP

namespace mcp {
    template<auto Handler>
    struct status_status_response_c : public detail::packet_base<0x00, Handler> {
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

            status_status_response_c::handler(base_handle, player_name, uuid);
        }
    };
}

#endif //MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP
