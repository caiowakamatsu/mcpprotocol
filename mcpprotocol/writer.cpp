#include "writer.hpp"

#include <bit>
#include <span>
#include <string_view>

namespace mcp {
    writer::writer(std::vector<std::byte> &vector) : vector(&vector) {

    }

    void writer::write(std::byte byte) {
        vector->push_back(byte);
    }

    void writer::write(std::span<const std::byte> bytes) {
        vector->insert(vector->end(), bytes.begin(), bytes.end());
    }

    void writer::write(bool value) {
        write(static_cast<std::uint8_t>(value));
    }

    void writer::write(std::string_view value) {
        write(mcp::var_int(static_cast<std::int32_t>(value.size())));
        write({reinterpret_cast<const std::byte*>(value.data()), value.size()});
    }

    void writer::write(mcp::var_int value) {
        auto as_uint = std::bit_cast<std::uint32_t>(value.value);
        do {
            auto temp = static_cast<std::uint8_t>(as_uint & 0b01111111);
            as_uint >>= 7;
            if (as_uint != 0) { temp |= 0b10000000; }
            write(std::byte(temp));
        } while (as_uint != 0);
    }

    void writer::write(mcp::var_long value) {
        auto as_uint = std::bit_cast<std::uint64_t>(value.value);
        do {
            auto temp = static_cast<std::uint8_t>(as_uint & 0b01111111);
            as_uint >>= 7;
            if (as_uint != 0) { temp |= 0b10000000; }
            write(std::byte(temp));
        } while (as_uint != 0);
    }
} // mcp
