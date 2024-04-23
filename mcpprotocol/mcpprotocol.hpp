#ifndef MCPPROTOCOL_MCPPROTOCOL_HPP
#define MCPPROTOCOL_MCPPROTOCOL_HPP

#include "version.hpp"
#include "network_state.hpp"
#include "writer.hpp"
#include "compression.hpp"

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
#include <iostream>

namespace mcp {
    struct packet_frame {
        std::uint32_t id;
        std::vector<std::byte> data;
    };

    enum class decode_status {
        eof,
        incomplete,
        state_changing,
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

        [[nodiscard]] static std::vector<std::byte> encode(auto &state, packet_frame &&frame) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            const auto packet_length = var_int(frame.data.size() + mcp::var_int(frame.id).size_bytes());

            // SupPorT OlD VerSioN Of MiNeCraFt "iTS GoOd prActIce"
            // - Statements made by the utterly deranged
            if (frame.id == 0xFE) {
                writer.write(std::uint8_t(0xFE));
                writer.write(frame.data);
                return buffer;
            }

            if (state.compression_threshold.has_value()) {
                if (packet_length.value >= state.compression_threshold.value()) {
                    auto uncompressed_buffer = std::vector<std::byte>();
                    auto compression_writer = mcp::writer(uncompressed_buffer);
                    compression_writer.write(mcp::var_int(frame.id));
                    compression_writer.write(frame.data);
                    auto compressed_buffer = mcp::compress(uncompressed_buffer);

                    writer.write(var_int(compressed_buffer.size() + packet_length.size_bytes()));
                    writer.write(var_int(packet_length));
                    writer.write(compressed_buffer);
                } else {
                    writer.write(var_int(packet_length.value + 1 /* +1 for the 0 byte of compressed length */));
                    writer.write(var_int(0));
                    writer.write(var_int(frame.id));
                    writer.write(frame.data);
                }
            } else {
                writer.write(packet_length);
                writer.write(mcp::var_int(frame.id));
                writer.write(frame.data);
            }

            return buffer;
        }

        template <template <auto _> typename Packet>
        [[nodiscard]] static std::vector<std::byte> serialize(auto &state, auto &&...args) {
            auto packet_frame = write<Packet>(std::forward<decltype(args)>(args)...);
            return encode(state, std::move(packet_frame));
        }

        template <typename ...Packets>
        struct deserializer {
            private:
                [[nodiscard]] std::vector<std::byte> handle_compression(std::optional<std::uint32_t> threshold, std::span<const std::byte> source) const {
                    auto reader = mcp::reader(source);

                    if (threshold.has_value()) {
                        const auto data_length = reader.read<mcp::var_int>().value;
                        if (data_length == 0) {
                            const auto rest = reader.remaining();
                            return { rest.begin(), rest.end() };
                        } else {
                            return mcp::decompress(reader.remaining(), data_length);
                        }
                    } else {
                        const auto rest = reader.remaining();
                        return { rest.begin(), rest.end() };
                    }
                }
            public:


            constexpr static std::uint32_t max_id = detail::max_packet_id<Packets...>::value;
            std::array<void *, max_id + 1> base_ptrs = {};

            template <typename ...HandlerBases>
            explicit deserializer(HandlerBases... bases) {
                (register_packet_base<Packets>(bases...), ...);
            }

            decode_status decode(auto &state, std::span<const std::byte> source) const {
                state.buffer.insert(state.buffer.end(), source.begin(), source.end());

                auto final_state = decode_status::eof;

                auto bytes_consumed = std::uint64_t(0);
                while (true) {
                    auto reader = mcp::reader({state.buffer.begin() + bytes_consumed, state.buffer.end()});
                    const auto length = reader.try_read_varint();

                    if (!length.has_value() || length->value > reader.remaining().size()) {
                        final_state = decode_status::incomplete;
                        break;
                    }
                    bytes_consumed += reader.save_cursor();
                    const auto packet_data = handle_compression(state.compression_threshold, reader.read_n(length->value));

                    auto packet_reader = mcp::reader(packet_data);
                    const auto packet_id = packet_reader.read<mcp::var_int>().value;


                    if (is_mode_switch_packet(state.mode, packet_id)) {
                        final_state = decode_status::state_changing;
                    }

                    [[maybe_unused]] const auto _ = ((
                            Packets::id == packet_id &&
                            ((Packets::template handle<Converters...>(get_member_base(Packets::id), packet_reader.remaining())), true))
                            || ...);

                    bytes_consumed += length->value;

                    if (final_state == decode_status::state_changing) {
                        break;
                    }
                }

                state.buffer.erase(state.buffer.begin(), state.buffer.begin() + bytes_consumed);
                return final_state;
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

            [[nodiscard]] static bool is_mode_switch_packet(stream_mode current_mode, std::uint32_t id) {
                switch (current_mode) {
                    case stream_mode::handshaking:
                        return true;
                    case stream_mode::status:
                        return false;
                    case stream_mode::configuration:
                        return id == 0x02; // Finish configuration
                    case stream_mode::login:
                        return id == 0x03; // Login acknowledged
                    case stream_mode::play:
                        return id == 0x67; // Start configuration
                    default:
                        return false; // unreachable
                }
            }
        };
    };
} // mcp

#endif //MCPPROTOCOL_MCPPROTOCOL_HPP
