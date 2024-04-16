#ifndef MCPPROTOCOL_STATUS_SERVER_BOUND_HPP
#define MCPPROTOCOL_STATUS_SERVER_BOUND_HPP

namespace mcp {
    template<auto Handler>
    struct status_status_request_s : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            status_status_request_s::handler(base_handle);
        }
    };
    template<auto Handler>
    struct status_ping_request_s : public detail::packet_base<0x01, Handler> {
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

            status_ping_request_s::handler(base_handle, payload);
        }
    };
}

#endif //MCPPROTOCOL_STATUS_SERVER_BOUND_HPP