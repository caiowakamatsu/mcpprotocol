#include <stdexcept>

#include "reader.hpp"

namespace mcp {
    reader::reader(std::span<const std::byte> buffer, std::uint64_t cursor) :
        buffer(buffer), cursor(cursor) {
    }

    std::span<const std::byte> reader::read_n(std::uint64_t count) {
        ensure_remaining(count);
        const auto old_cursor = static_cast<std::int64_t>(cursor);
        cursor += count;
        return { buffer.begin() + old_cursor, count };
    }

    void reader::ensure_remaining(std::uint64_t count) {
        const auto remaining = buffer.size() - cursor;
        if (count > remaining) {
            throw std::runtime_error("tried to read more bytes than available");
        }
    }

    template <>
    mcp::var_int reader::read<mcp::var_int>() {
        auto num_read = std::uint32_t(0);
        auto result   = std::int32_t(0);
        auto read     = std::uint8_t(0);

        do {
            read                = static_cast<std::uint8_t>(buffer[cursor]);
            std::uint32_t value = (read & 0b01111111);
            result |= std::bit_cast<std::int32_t>((value << (7 * num_read)));

            num_read++;
            cursor++;
        } while ((read & 0b10000000) != 0);

        return mcp::var_int(result);
    }

    template <>
    mcp::var_long reader::read<mcp::var_long>() {
        auto num_read = std::uint64_t(0);
        auto result   = std::int64_t(0);
        auto read     = std::uint8_t(0);

        do {
            read                = static_cast<std::uint8_t>(buffer[cursor]);
            std::uint64_t value = (read & 0b01111111);
            result |= std::bit_cast<std::int64_t>((value << (7 * num_read)));

            num_read++;
            cursor++;
        } while ((read & 0b10000000) != 0);

        return mcp::var_long(result);
    }

    std::span<const std::byte> reader::remaining() const {
        return {buffer.data() + cursor, buffer.size() - cursor};
    }

} // mcp