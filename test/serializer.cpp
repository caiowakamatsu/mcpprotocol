#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/reader.hpp>

void ensure_buffer_ok(std::span<const std::byte> bytes, std::initializer_list<std::uint8_t> expected);

struct basic_packet {
    constexpr static std::uint32_t id = 0x69;
    [[nodiscard]] static mcp::outbound_packet_frame write(int foo, std::string_view bar) {
        auto frame = mcp::outbound_packet_frame();
        frame.id = id;
        auto writer = mcp::writer(frame.data);

        writer.write(foo);
        writer.write(bar);

        return frame;
    }
};

TEST_CASE("Basic packet", "[serializer]") {
    using protocol = mcp::versioned_protocol<mcp::version::v765>;

    const auto bytes = protocol::serialize(basic_packet::write(5, "Hello World!"));
    ensure_buffer_ok(bytes, {
            0x69,  // packet id
            0x11,  // data length as varint, 17 bytes
            0x00, 0x00, 0x00, 0x05,
            0x0c, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'
    });
}
