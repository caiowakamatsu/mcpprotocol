#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "network_state.hpp"
#include "writer.hpp"

#include "packets/config_client_bound.hpp"
#include "packets/config_server_bound.hpp"
#include "packets/handshake_server_bound.hpp"
#include "packets/handshake_client_bound.hpp"
#include "packets/login_client_bound.hpp"
#include "packets/login_server_bound.hpp"
#include "packets/play_client_bound.hpp"
#include "packets/play_server_bound.hpp"
#include "packets/status_client_bound.hpp"
#include "packets/status_server_bound.hpp"

namespace mcp {
    struct packet_frame {
        std::uint32_t id;
        std::vector<std::byte> data;
    };

    template <mcp::version version, typename ...Converters>
    struct protocol {
        template <template <auto _> typename Packet>
        [[nodiscard]] static packet_frame write(auto &&...args) {
            return {
                .id = Packet<0>::id,
                .data = Packet<0>::template serialize<Converters...>(std::forward<decltype(args)>(args)...)
            };
        }

        template <template <auto _> typename Packet>
        [[nodiscard]] static std::vector<std::byte> encode(auto &state, packet_frame &&frame) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            const auto id = var_int(static_cast<std::int32_t>(frame.id));
            const auto packet_length = var_int(static_cast<std::int32_t>(frame.data.size()) + id.size_bytes());

            // SupPorT OlD VerSioN Of MiNeCraFt "iTS GoOd prActIce"
            // - Statements made by the utterly deranged
            if constexpr (Packet<0>::id == 0xFE) {
                writer.write(std::uint8_t(id.value));
                writer.write(frame.data);
                return buffer;
            }

            writer.write(packet_length);
            writer.write(id);
            writer.write(frame.data);
        }

        template <template <auto _> typename Packet>
        [[nodiscard]] static std::vector<std::byte> serialize(auto &state, auto &&...args) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            constexpr auto id = var_int(Packet<0>::id);
            const auto data = Packet<0>::template serialize<Converters...>(std::forward<decltype(args)>(args)...);
            const auto packet_length = var_int(data.size() + id.size_bytes());

            // SupPorT OlD VerSioN Of MiNeCraFt "iTS GoOd prActIce"
            // - Statements made by the utterly deranged
            if constexpr (id.value == 0xFE) {
                writer.write(std::uint8_t(id.value));
                writer.write(data);
                return buffer;
            }

            writer.write(packet_length);
            writer.write(id);
            writer.write(data);

            return buffer;
        }

        template <typename ...Packets>
        struct deserializer {
            constexpr static std::uint32_t max_id = detail::max_packet_id<Packets...>::value;
            std::array<void *, max_id + 1> base_ptrs = {};

            template <typename ...HandlerBases>
            explicit deserializer(HandlerBases... bases) {
                (register_packet_base<Packets>(bases...), ...);
            }

            void decode(auto &state, std::span<const std::byte> source) const {

                // TODO: this can probably be faster with buffer reuse and raw memcpy
                auto reconstructed_stream = std::vector<std::byte>();
                reconstructed_stream.reserve(state.previous_partial_packet.size() + source.size());
                reconstructed_stream.insert(reconstructed_stream.end(), state.previous_partial_packet.begin(), state.previous_partial_packet.end());
                reconstructed_stream.insert(reconstructed_stream.end(), source.begin(), source.end());
                state.previous_partial_packet.clear();

                auto reader = mcp::reader(reconstructed_stream);
                while (true) {
                    const auto packet_start = reader.save_cursor();
                    const auto maybe_length = reader.try_read_varint();
                    if(!maybe_length.has_value() || maybe_length.value().value > reader.remaining().size()) {
                        // we have an incomplete packet here
                        reader.restore_cursor(packet_start);
                        state.previous_partial_packet.insert(state.previous_partial_packet.end(), reader.remaining().begin(), reader.remaining().end());
                        break;
                    }

                    const auto id = reader.read<var_int>().value;
                    [[maybe_unused]] const auto _ = ((
                            Packets::id == id &&
                            ((Packets::template handle<Converters...>(get_member_base(Packets::id), reader.read_n(maybe_length->value))), true))
                            || ...);
                }
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
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
