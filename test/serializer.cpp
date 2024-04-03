#include <catch2/catch_all.hpp>

#include <mcpprotocol/mcpprotocol.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/packet_frame.hpp>

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
    // ensure the bytes are what's expected
}
