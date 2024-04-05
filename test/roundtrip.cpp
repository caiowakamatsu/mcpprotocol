#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/packets/login_server_bound.hpp>

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

TEST_CASE("Conversion Test", "[roundtrip]") {
    struct my_uuid {
        std::uint64_t low;
        std::uint64_t high;
    };

    struct uuid_converter {
        using type_source = mcp::uuid;
        using type_target = my_uuid;

        [[nodiscard]] static type_target from(mcp::reader &reader) {
            auto uuid = my_uuid();
            uuid.low = reader.read<std::uint64_t>();
            uuid.high = reader.read<std::uint64_t>();
            return uuid;
        }

        static void to(my_uuid source, mcp::writer &writer) {
            writer.write(source.low);
            writer.write(source.high);
        }
    };

    using protocol = mcp::protocol<mcp::version::v765, uuid_converter>;

    struct login_start_s {
        void handle(std::string name, my_uuid uuid) const {
            REQUIRE(name == "_NotLegend");
            REQUIRE(uuid.low == 0x6969);
            REQUIRE(uuid.high == 0x420);
        }
    } deserializer_test;

    const auto test_deserializer = protocol::deserializer<
            mcp::login_start_s<&login_start_s::handle>
    >(&deserializer_test);

    auto network_state = mcp::basic_network_state<void>();

    const auto bytes = protocol::serialize<mcp::login_start_s>(network_state,
                                                             std::string("_NotLegend"),
                                                             my_uuid { .low = 0x6969, .high = 0x420 });

    network_state = mcp::basic_network_state<void>();
    test_deserializer.decode(network_state, bytes);
}