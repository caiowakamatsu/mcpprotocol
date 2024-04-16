#ifndef MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP
#define MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP

namespace mcp {
    template<auto Handler>
    struct status_status_response_c : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string json_response) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(json_response);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto json_response = reader.read<std::string>();

            status_status_response_c::handler(base_handle, json_response);
        }
    };


    template<auto Handler>
    struct status_ping_response_c : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint64_t payload) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(payload);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto payload = reader.read<std::uint64_t>();

            status_ping_response_c::handler(base_handle, payload);
        }
    };
}

#endif //MCPPROTOCOL_STATUS_CLIENT_BOUND_HPP
