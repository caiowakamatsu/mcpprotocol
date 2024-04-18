#ifndef MCPPROTOCOL_PACKET_BASE_HPP
#define MCPPROTOCOL_PACKET_BASE_HPP

#include <mcpprotocol/packet_traits.hpp>

#include <tuple>

namespace mcp::detail {
    template <std::uint32_t PacketID, auto ...Handlers>
    struct packet_base {
        using FirstHandler = std::tuple_element_t<0, std::tuple<decltype(Handlers)...>>;
        constexpr static std::uint32_t id = PacketID;
        using handle_t = FirstHandler;
        constexpr static bool is_free_function = !std::is_member_function_pointer<FirstHandler>::value;

        template <typename BaseHandler, auto Handler, typename ...Args>
        static void try_apply(BaseHandler *base, Args ...args) {
            if constexpr (requires { std::invoke(Handler, std::forward<Args...>(args...)); } ) {
                if constexpr (is_free_function) {
                    handler(base, std::forward<Args>(args)...);
                } else {
                    (reinterpret_cast<detail::base_class_of_function<handle_t>::type*>(base)->*Handler)(std::forward<Args>(args)...);
                }
            }
        }

        template <typename BaseHandler, typename ...Args>
        static void handler(BaseHandler *base, Args ...args) {
            (try_apply<BaseHandler, Handlers>(base, std::forward<Args>(args)...), ...);
        }
    };

    template <std::uint32_t PacketId, auto Handler>
    struct packet_base<PacketId, Handler> {
        constexpr static std::uint32_t id = PacketId;
        using handle_t = decltype(Handler);
        constexpr static bool is_free_function = !std::is_member_function_pointer<decltype(Handler)>::value;

        template <typename BaseHandler, typename ...Args>
        static void handler(BaseHandler *base, Args ...args) {
            if constexpr (is_free_function) {
                Handler(args...);
            } else {
                (reinterpret_cast<detail::base_class_of_function<handle_t>::type*>(base)->*Handler)(std::forward<Args>(args)...);
            }
        }
    };
}

#endif //MCPPROTOCOL_PACKET_BASE_HPP
