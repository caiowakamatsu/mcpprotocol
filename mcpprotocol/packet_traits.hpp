#ifndef MCPPROTOCOL_PACKET_TRAITS_HPP
#define MCPPROTOCOL_PACKET_TRAITS_HPP

#include <type_traits>

namespace mcp::detail {
    template <typename SourceType, typename ...Converters>
    struct get_type;

    template <typename SourceType, typename Converter, typename ...Converters>
    struct get_type<SourceType, Converter, Converters...> {
        using type = std::conditional_t<std::is_same_v<typename Converter::type_source, SourceType>,
                Converter,
                typename get_type<SourceType, Converters...>::type>;
    };

    template <typename SourceType>
    struct get_type<SourceType> {
        using type = void;
    };

    template <typename SourceType, typename ...Converters>
    using get_type_t = typename get_type<SourceType, Converters...>::type;

    template <typename T>
    struct base_class_of_function;

    template <typename ClassType, typename ReturnType, typename... Args>
    struct base_class_of_function<ReturnType(ClassType::*)(Args...)> {
        using type = ClassType;
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    struct base_class_of_function<ReturnType(ClassType::*)(Args...) const> {
        using type = ClassType;
    };

    template <typename... Packets>
    struct max_packet_id;

    template <typename Packet>
    struct max_packet_id<Packet> {
        constexpr static std::uint32_t value = Packet::id;
    };

    template <typename Packet, typename ...Packets>
    struct max_packet_id<Packet, Packets...> {
        constexpr static std::uint32_t value =
                (Packet::id > max_packet_id<Packets...>::value) ? Packet::id : max_packet_id<Packets...>::value;
    };

    template <>
    struct max_packet_id<> {
        static constexpr std::uint32_t value = 0;
    };
}

#endif //MCPPROTOCOL_PACKET_TRAITS_HPP
