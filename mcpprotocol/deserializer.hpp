#ifndef MCPPROTOCOL_DESERIALIZER_HPP
#define MCPPROTOCOL_DESERIALIZER_HPP

#include <cstdint>
#include <vector>

#include <mcpprotocol/packet_traits.hpp>

namespace mcp {
    template <typename UserData>
    struct basic_network_state {
        int counter = 0;
        UserData user_data;
    };

    template <>
    struct basic_network_state<void> {
        int counter = 0;
    };

    template <typename ...Converters>
    struct basic_deserializer {
        template <typename ...Packets>
        struct deserializer_implementation {
            constexpr static std::uint32_t max_id = detail::max_packet_id<Packets...>::value;
            std::array<void *, max_id + 1> base_ptrs = {};

            template <typename ...HandlerBases>
            explicit deserializer_implementation(HandlerBases... bases) {
                (register_packet_base<Packets>(bases...), ...);
            }

            void ingest(std::string_view source, auto &state) const {
                const auto id = source[0] - '0';
                ((Packets::id == id && Packets::template handle<Converters...>(get_member_base(Packets::id), &state, {source.begin() + 1, source.end()})) || ...);
            }

        private:
            template <typename BaseType, typename PacketBase>
            void* get_matching_base(PacketBase base) {
                if (std::is_same<typename std::remove_pointer<PacketBase>::type, BaseType>::value) {
                    return base;
                }
                throw std::runtime_error("Didn't specify base for all instanced callbacks");
            }

            template <typename BaseType, typename PacketBase, typename ...PacketBases>
            void* get_matching_base(PacketBase base, PacketBases... bases) {
                if (std::is_same<typename std::remove_pointer<PacketBase>::type, BaseType>::value) {
                    return base;
                }
                return get_matching_base<BaseType>(bases...);
            }

            template <typename PacketT, typename ...PacketBases>
            void register_packet_base(PacketBases ...bases) {
                if constexpr (!PacketT::is_free_function) {
                    const auto id = PacketT::id;
                    base_ptrs[id] = reinterpret_cast<void*>(get_matching_base<typename detail::base_class_of_function<typename PacketT::handle_t>::type>(bases...));
                }
            }

            [[nodiscard]] void* get_member_base(std::uint32_t id) const {
                return base_ptrs[id];
            }
        };

        template <typename ...Packets>
        using with_packets = deserializer_implementation<Packets...>;
    };
} // mcp

#endif //MCPPROTOCOL_DESERIALIZER_HPP
