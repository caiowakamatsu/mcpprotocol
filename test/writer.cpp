#include <catch2/catch_all.hpp>

#include <mcpprotocol/writer.hpp>

void ensure_buffer_ok(std::span<const std::byte> bytes, std::initializer_list<std::uint8_t> expected) {
    REQUIRE(bytes.size() == expected.size());
    for (size_t i = 0; i < bytes.size(); i++) {
        REQUIRE(bytes[i] == std::byte(*(expected.begin() + i)));
    }
}

void ensure_buffer_ok(std::span<const std::byte> bytes, std::span<std::byte> expected) {
    REQUIRE(bytes.size() == expected.size());
    for (size_t i = 0; i < bytes.size(); i++) {
        REQUIRE(bytes[i] == expected[i]);
    }
}

TEST_CASE("Creation", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);
}

TEST_CASE("Byte", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(std::byte(0x69));
    ensure_buffer_ok(buffer, { 0x69 });
}

TEST_CASE("Bytes", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    auto random_bytes = std::array<std::byte, 512>();
    for (int i = 0; i < random_bytes.size(); i++) {
        random_bytes[i] = std::byte(i % 149);
    }

    writer.write(random_bytes);
    for (int i = 0; i < random_bytes.size(); i++) {
        REQUIRE(buffer[i] == random_bytes[i]);
    }
}

TEST_CASE("Integral", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(std::uint8_t(0x01));
    writer.write(std::uint16_t(0x0102));
    writer.write(std::uint32_t(0x01020304));
    writer.write(std::uint64_t(0x0102030405060708ull));
    ensure_buffer_ok(buffer, {
            0x01,
            0x01, 0x02,
            0x01, 0x02, 0x03, 0x04,
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    });
}

TEST_CASE("Floating", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(std::float_t(3.14f));
    writer.write(std::double_t(123.456l));
    ensure_buffer_ok(buffer, {
            0x40, 0x48, 0xf5, 0xc3,
            0x40, 0x5e, 0xdd, 0x2f, 0x1a, 0x9f, 0xbe, 0x77
    });

}

TEST_CASE("Bool", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(true);
    writer.write(false);

    REQUIRE(buffer[0] == std::byte(0x01));
    REQUIRE(buffer[1] == std::byte(0x00));
}

TEST_CASE("String", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    const auto str = std::string("Hello World!");

    writer.write(str);

    REQUIRE(buffer[0] == std::byte(0x0C));
    for (int i = 0; i < str.size(); i++) {
        REQUIRE(buffer[i + 1] == std::byte(str[i]));
    }
}

TEST_CASE("VarInt", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(mcp::var_int(0));
    writer.write(mcp::var_int(1));
    writer.write(mcp::var_int(2));
    writer.write(mcp::var_int(127));
    writer.write(mcp::var_int(128));
    writer.write(mcp::var_int(255));
    writer.write(mcp::var_int(25565));
    writer.write(mcp::var_int(2097151));
    writer.write(mcp::var_int(2147483647));
    writer.write(mcp::var_int(-1));
    writer.write(mcp::var_int(-2147483648));
    ensure_buffer_ok(buffer,{
            0x00,
            0x01,
            0x02,
            0x7f,
            0x80, 0x01,
            0xff, 0x01,
            0xdd, 0xc7, 0x01,
            0xff, 0xff, 0x7f,
            0xff, 0xff, 0xff, 0xff, 0x07,
            0xff, 0xff, 0xff, 0xff, 0x0f,
            0x80, 0x80, 0x80, 0x80, 0x08
    });
}

TEST_CASE("VarLong", "[writer]") {
    auto buffer = std::vector<std::byte>();
    auto writer = mcp::writer(buffer);

    writer.write(mcp::var_long(0));
    writer.write(mcp::var_long(1));
    writer.write(mcp::var_long(2));
    writer.write(mcp::var_long(127));
    writer.write(mcp::var_long(128));
    writer.write(mcp::var_long(255));
    writer.write(mcp::var_long(2147483647));
    writer.write(mcp::var_long(9223372036854775807));

    ensure_buffer_ok(buffer, {
        0x00,
        0x01,
        0x02,
        0x7f,
        0x80, 0x01,
        0xff, 0x01,
        0xff, 0xff, 0xff, 0xff, 0x07,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f });
}