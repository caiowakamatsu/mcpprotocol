#ifndef MCPPROTOCOL_TYPES_HPP
#define MCPPROTOCOL_TYPES_HPP

#include <cstdint>


namespace mcp {
    struct var_int {
        std::int32_t value;

        [[nodiscard]] inline std::int32_t size_bytes() const noexcept
        {
            const auto x = std::bit_cast<std::uint32_t>(value);
            if (x <= 0x7F) return 1;
            if (x <= 0x3FFF) return 2;
            if (x <= 0x1FFFFF) return 3;
            if (x <= 0xFFFFFFF) return 4;
            return 5;
        }
    };
    struct var_long { std::int64_t value; };

}
#endif //MCPPROTOCOL_TYPES_HPP
