#ifndef MCPPROTOCOL_PACKET_BASE_HPP
#define MCPPROTOCOL_PACKET_BASE_HPP

#include <mcpprotocol/packet_traits.hpp>

namespace mcp {
    template <auto Handler>
    struct packet_base {
        using handle_t = decltype(Handler);
        constexpr static bool is_free_function = !std::is_member_function_pointer<decltype(Handler)>::value;

        template <typename BaseHandler, typename ...Args>
        static void handler(BaseHandler *base, Args ...args) {
            if constexpr (is_free_function) {
                Handler(args...);
            } else {
                (reinterpret_cast<detail::base_class_of_function<handle_t>::type*>(base)->*Handler)(args...);
            }
        }
    };
}

#endif //MCPPROTOCOL_PACKET_BASE_HPP
