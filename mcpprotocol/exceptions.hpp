#ifndef MCPPROTOCOL_EXCEPTIONS_HPP
#define MCPPROTOCOL_EXCEPTIONS_HPP

#include <exception>

namespace mcp {
    struct varint_read_exception : public std::exception {
        struct too_long;
        struct incomplete;

        [[nodiscard]] const char* what() const noexcept override = 0;
    };

    struct varint_read_exception::too_long : public varint_read_exception {
        [[nodiscard]] const char* what() const noexcept override {
            return "varint too long";
        }
    };

    struct varint_read_exception::incomplete : public varint_read_exception {
        [[nodiscard]] const char* what() const noexcept override {
            return "varint read reached end of buffer with continuation bit active";
        }
    };
}
#endif //MCPPROTOCOL_EXCEPTIONS_HPP
