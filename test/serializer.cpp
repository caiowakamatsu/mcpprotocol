#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/writer.hpp>

void ensure_buffer_ok(std::span<const std::byte> bytes, std::initializer_list<std::uint8_t> expected);

TEST_CASE("Basic packet", "[serializer]") {
    using protocol = mcp::protocol<mcp::version::v765>;

//    const auto bytes = protocol::serialize(basic_packet::write(5, "Hello World!"));
//    ensure_buffer_ok(bytes, {
//            0x69,  // packet id
//            0x11,  // data length as varint, 17 bytes
//            0x00, 0x00, 0x00, 0x05,
//            0x0c, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'
//    });
}
