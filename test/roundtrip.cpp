#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/network_state.hpp>
#include <mcpprotocol/packets/handshake_server_bound.hpp>

TEST_CASE("Legacy Ping", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct handshake_s {
        void handle_handshake_s(std::int32_t protocol_version, std::string server_name, std::uint16_t port, std::uint8_t next_state) const {
            REQUIRE(protocol_version == 765);
            REQUIRE(server_name == "localhost");
            REQUIRE(port == 25565);
            REQUIRE(next_state == 1);
        }
    } deserializer_test;

    const auto test_deserializer = protocol::deserializer<
            mcp::handshake_s<&handshake_s::handle_handshake_s>
    >(&deserializer_test);

    auto network_state = mcp::basic_network_state<void>();

    REQUIRE(network_state.mode == mcp::stream_mode::handshaking);
    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));
    REQUIRE(network_state.mode == mcp::stream_mode::status);

    network_state = mcp::basic_network_state<void>();
    REQUIRE(network_state.mode == mcp::stream_mode::handshaking);
    test_deserializer.decode(network_state, bytes);
    REQUIRE(network_state.mode == mcp::stream_mode::status);
}