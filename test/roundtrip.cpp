#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/network_state.hpp>
#include <mcpprotocol/packets/handshake_server_bound.hpp>

TEST_CASE("Legacy Ping", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;
    auto network_state = mcp::basic_network_state<void>();

    struct legacy_ping_s {
        std::uint8_t expected_payload = 0x01;

        void handle_legacy_ping_s(std::uint8_t payload) const {
            REQUIRE(payload == expected_payload);
        }
    } deserializer_test;

    const auto bytes = protocol::serialize<mcp::legacy_ping_s>(std::uint8_t(0x01), network_state);
    REQUIRE(bytes[0] == std::byte(0xFE));
    REQUIRE(bytes[1] == std::byte(0x01));

    const auto test_deserializer = protocol::deserializer<
                                    mcp::legacy_ping_s<legacy_ping_s::handle_legacy_ping_s>,
    >(&deserializer_test);

    test_deserializer.deserialize(bytes, network_state);
}