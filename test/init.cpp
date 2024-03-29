#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>

TEST_CASE("Basic", "[init]") {
    using protocol = mcp::versioned_protocol<mcp::version::v765>;

    auto serializer = protocol::serializer();
    auto deserializer = protocol::deserializer();
}
