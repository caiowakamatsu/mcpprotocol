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

} // mcp