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

    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));

    network_state = mcp::basic_network_state<void>();
    test_deserializer.decode(network_state, bytes);
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

TEST_CASE("Multiple packets", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct handshake_s {
        int count = 0;
        void handle_handshake_s(std::int32_t protocol_version, std::string server_name, std::uint16_t port, std::uint8_t next_state) {
            count += 1;
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

    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));

    test_deserializer.decode(network_state, bytes);
    test_deserializer.decode(network_state, bytes);
    REQUIRE(deserializer_test.count == 2);
}

TEST_CASE("Multiple packets one decode", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct handshake_s {
        int count = 0;
        void handle_handshake_s(std::int32_t protocol_version, std::string server_name, std::uint16_t port, std::uint8_t next_state) {
            count += 1;
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

    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));

    auto actual_bytes = std::vector<std::byte>();
    actual_bytes.insert(actual_bytes.end(), bytes.begin(), bytes.end());
    actual_bytes.insert(actual_bytes.end(), bytes.begin(), bytes.end());

    test_deserializer.decode(network_state, actual_bytes);
    test_deserializer.decode(network_state, actual_bytes);
    REQUIRE(deserializer_test.count == 2);
}

TEST_CASE("Multiple packets two decodes", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct keep_alive_c {
        int count = 0;
        void handle(std::uint64_t payload) {
            count += 1;
            REQUIRE(payload == 0xCAFEBABE);
        }
    } deserializer_test;

    const auto test_deserializer = protocol::deserializer<
            mcp::play_keep_alive_c<&keep_alive_c::handle>
    >(&deserializer_test);

    auto network_state = mcp::basic_network_state<void>();
    network_state.mode = mcp::stream_mode::play;

    const auto bytes = protocol::serialize<mcp::play_keep_alive_c>(network_state, std::uint64_t(0xCAFEBABE));

    auto actual_bytes = std::vector<std::byte>();
    actual_bytes.insert(actual_bytes.end(), bytes.begin(), bytes.end());
    actual_bytes.insert(actual_bytes.end(), bytes.begin(), bytes.end());
    actual_bytes.insert(actual_bytes.end(), bytes.begin(), bytes.begin() + 5);

    test_deserializer.decode(network_state, actual_bytes);
    REQUIRE(deserializer_test.count == 2);
    actual_bytes.clear();

    actual_bytes.insert(actual_bytes.end(), bytes.begin() + 5, bytes.end());
    test_deserializer.decode(network_state, actual_bytes);
    REQUIRE(deserializer_test.count == 3);


}

TEST_CASE("Single packet with compression", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct handshake_s {
        int count = 0;
        void handle_handshake_s(std::int32_t protocol_version, std::string server_name, std::uint16_t port, std::uint8_t next_state) {
            count += 1;
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
    network_state.compression_threshold = 0;

    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));

    test_deserializer.decode(network_state, bytes);
    REQUIRE(deserializer_test.count == 1);
}

TEST_CASE("Single packet under compression threshold", "[roundtrip]") {
    using protocol = mcp::protocol<mcp::version::v765>;

    struct handshake_s {
        int count = 0;
        void handle_handshake_s(std::int32_t protocol_version, std::string server_name, std::uint16_t port, std::uint8_t next_state) {
            count += 1;
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
    network_state.compression_threshold = 0x7FFFFFFF;  // some huge number we'll never reach

    const auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             std::int32_t(765),
                                                             std::string("localhost"),
                                                             std::uint16_t(25565),
                                                             std::uint8_t(1));

    test_deserializer.decode(network_state, bytes);
    REQUIRE(deserializer_test.count == 1);
}
