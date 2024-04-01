#ifndef MCPPROTOCOL_TYPES_HPP
#define MCPPROTOCOL_TYPES_HPP

#include <cstdint>


namespace mcp {
    struct var_int { std::int32_t value; };
    struct var_long { std::int64_t value; };

}
#endif //MCPPROTOCOL_TYPES_HPP
