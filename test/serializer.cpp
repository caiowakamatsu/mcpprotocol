#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/reader.hpp>

struct basic_packet {
    constexpr static std::uint32_t id = 0x69;
    int foo = 5;
    std::string bar = "hello world";

    void write(mcp::writer writer) {
        writer.write(foo);
        writer.write(bar);
    }
};

TEST_CASE("Basic packet", "[serializer]") {
    using protocol = mcp::versioned_protocol<mcp::version::v765>;

    auto packet = basic_packet();
    const auto bytes = protocol::serialize(packet);
}
