#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>

TEST_CASE("Basic", "[init]") {
    using protocol = mcp::protocol<mcp::version::v765>;
}
