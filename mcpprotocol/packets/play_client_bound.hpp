#ifndef MCPPROTOCOL_PLAY_CLIENT_BOUND_HPP
#define MCPPROTOCOL_PLAY_CLIENT_BOUND_HPP

#include <mcpprotocol/types.hpp>
#include <mcpprotocol/packet_base.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/writer.hpp>

namespace mcp {
    template<auto Handler>
    struct play_bundle_delimiter_c : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            play_bundle_delimiter_c::handler(base_handle);
        }
    };

    template<auto Handler>
    struct play_spawn_entity_c : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                mcp::var_int type,
                double x,
                double y,
                double z,
                std::uint8_t pitch,
                std::uint8_t yaw,
                std::uint8_t head_yaw,
                mcp::var_int data,
                std::int16_t velocity_x,
                std::int16_t velocity_y,
                std::int16_t velocity_z) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(type);
            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(pitch);
            writer.write(yaw);
            writer.write(head_yaw);
            writer.write(data);
            writer.write(velocity_x);
            writer.write(velocity_y);
            writer.write(velocity_z);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto type = reader.read<mcp::var_int>();
            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto pitch = reader.read<std::uint8_t>();
            const auto yaw = reader.read<std::uint8_t>();
            const auto head_yaw = reader.read<std::uint8_t>();
            const auto data = reader.read<mcp::var_int>();
            const auto velocity_x = reader.read<std::int16_t>();
            const auto velocity_y = reader.read<std::int16_t>();
            const auto velocity_z = reader.read<std::int16_t>();

            play_spawn_entity_c::handler(base_handle,
                                         entity_id,
                                         uuid,
                                         type,
                                         x,
                                         y,
                                         z,
                                         pitch,
                                         yaw,
                                         head_yaw,
                                         data,
                                         velocity_x,
                                         velocity_y,
                                         velocity_z);
        }
    };

    template<auto Handler>
    struct play_spawn_experience_orb_c : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                double x,
                double y,
                double z,
                std::int16_t count) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(count);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto count = reader.read<std::int16_t>();

            play_spawn_experience_orb_c::handler(base_handle, entity_id, x, y, z, count);
        }
    };

    template<auto Handler>
    struct play_entity_animation_c : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::uint8_t animation) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(animation);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto animation = reader.read<std::uint8_t>();

            play_entity_animation_c::handler(base_handle, entity_id, animation);
        }
    };

    struct play_statistics_entry {
        mcp::var_int category_id;
        mcp::var_int statistic_id;
        mcp::var_int value;
    };
    template<auto Handler>
    struct play_award_statistics_c : public detail::packet_base<0x04, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<play_statistics_entry> entries) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(entries.size()));
            for (const auto &entry : entries) {
                writer.write(entry.category_id);
                writer.write(entry.statistic_id);
                writer.write(entry.value);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entries_count = reader.read<mcp::var_int>();
            std::vector<play_statistics_entry> entries;
            entries.reserve(entries_count.value);

            for (std::size_t i = 0; i < entries_count.value; ++i) {
                play_statistics_entry entry;
                entry.category_id = reader.read<mcp::var_int>();
                entry.statistic_id = reader.read<mcp::var_int>();
                entry.value = reader.read<mcp::var_int>();
                entries.push_back(entry);
            }

            play_award_statistics_c::handler(base_handle, entries);
        }
    };

    template<auto Handler>
    struct play_acknowledge_block_change_c : public detail::packet_base<0x05, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int sequence_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(sequence_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto sequence_id = reader.read<mcp::var_int>();

            play_acknowledge_block_change_c::handler(base_handle, sequence_id);
        }
    };

    template<auto Handler>
    struct play_set_block_destroy_stage_c : public detail::packet_base<0x06, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                std::int8_t stage) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(stage);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto stage = reader.read<std::int8_t>();

            play_set_block_destroy_stage_c::handler(base_handle, entity_id, position, stage);
        }
    };

    template<auto Handler>
    struct play_block_entity_data_c : public detail::packet_base<0x07, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                mcp::var_int type,
                detail::get_type_t<mcp::nbt, Converters...>::type_target nbt) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(type);
            detail::get_type_t<mcp::nbt, Converters...>::to(nbt, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto type = reader.read<mcp::var_int>();
            auto nbt = detail::get_type_t<mcp::nbt, Converters...>::from(reader);

            play_block_entity_data_c::handler(base_handle, position, type, nbt);
        }
    };

    template<auto Handler>
    struct play_block_action_c : public detail::packet_base<0x08, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                std::int8_t action_id,
                std::int8_t action_param,
                mcp::var_int block_type) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(action_id);
            writer.write(action_param);
            writer.write(block_type);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto action_id = reader.read<std::int8_t>();
            const auto action_param = reader.read<std::int8_t>();
            const auto block_type = reader.read<mcp::var_int>();

            play_block_action_c::handler(base_handle, position, action_id, action_param, block_type);
        }
    };

    template<auto Handler>
    struct play_block_update_c : public detail::packet_base<0x09, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                mcp::var_int block_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(block_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto block_id = reader.read<mcp::var_int>();

            play_block_update_c::handler(base_handle, position, block_id);
        }
    };

    template<auto Handler>
    struct play_boss_bar_c : public detail::packet_base<0x0A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                detail::get_type_t<mcp::text_component, Converters...>::type_target title,
                float health,
                mcp::var_int colour,
                mcp::var_int division,
                std::uint8_t flags) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(0)); // Action: add
            detail::get_type_t<mcp::text_component, Converters...>::to(title, writer);
            writer.write(health);
            writer.write(colour);
            writer.write(division);
            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(1)); // Action: remove

            return buffer;
        }

        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                float health) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(2)); // Action: update health
            writer.write(health);

            return buffer;
        }

        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                detail::get_type_t<mcp::text_component, Converters...>::type_target title) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(3)); // Action: update title
            detail::get_type_t<mcp::text_component, Converters...>::to(title, writer);

            return buffer;
        }

        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                mcp::var_int colour,
                mcp::var_int dividers) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(4)); // Action: update style
            writer.write(colour);
            writer.write(dividers);

            return buffer;
        }

        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                std::int8_t flags) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(mcp::var_int(5)); // Action: update flags
            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto action = reader.read<mcp::var_int>().value;
            if (action == 0) {
                auto title = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
                auto health = reader.read<float>();
                auto colour = reader.read<mcp::var_int>();
                auto division = reader.read<mcp::var_int>();
                const auto flags = reader.read<std::int8_t>();
                play_boss_bar_c::handler(base_handle, uuid, title, health, colour, division, flags);
            } else if (action == 1) {
                play_boss_bar_c::handler(base_handle, uuid);
            } else if (action == 2) {
                auto health = reader.read<float>();
                play_boss_bar_c::handler(base_handle, uuid, health);
            } else if (action == 3) {
                auto title = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
                play_boss_bar_c::handler(base_handle, uuid, title);
            } else if (action == 4) {
                auto colour = reader.read<mcp::var_int>();
                auto division = reader.read<mcp::var_int>();
                play_boss_bar_c::handler(base_handle, uuid, colour, division);
            } else if (action == 5) {
                auto flags = reader.read<std::int8_t>();
                play_boss_bar_c::handler(base_handle, uuid, flags);
            } else {
                throw std::runtime_error("Unknown boss bar action");
            }
        }
    };

    template<auto Handler>
    struct play_change_difficulty_c : public detail::packet_base<0x0B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t difficulty,
                bool locked) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(difficulty);
            writer.write(locked);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto difficulty = reader.read<std::uint8_t>();
            const auto locked = reader.read<bool>();

            play_change_difficulty_c::handler(base_handle, difficulty, locked);
        }
    };

    template<auto Handler>
    struct play_chunk_batch_finished_c : public detail::packet_base<0x0C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int batch_size) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(batch_size);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto batch_size = reader.read<mcp::var_int>();

            play_chunk_batch_finished_c::handler(base_handle, batch_size);
        }
    };

    template<auto Handler>
    struct play_chunk_batch_start : public detail::packet_base<0x0D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            play_chunk_batch_start::handler(base_handle);
        }
    };

    struct play_chunk_biomes_c_biome_data {
        std::int32_t z;
        std::int32_t x;
        mcp::var_int size;
        std::vector<std::byte> data;
    };
    template<auto Handler>
    struct play_chunk_biomes_c : public detail::packet_base<0x0E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<play_chunk_biomes_c_biome_data> biomes) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(biomes.size()));
            for (const auto &biome : biomes) {
                writer.write(biome.z);
                writer.write(biome.x);
                writer.write(biome.size);
                writer.write(biome.data);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto biomes_count = reader.read<mcp::var_int>();
            std::vector<play_chunk_biomes_c_biome_data> biomes;
            biomes.reserve(biomes_count.value);

            for (std::size_t i = 0; i < biomes_count.value; ++i) {
                play_chunk_biomes_c_biome_data biome;
                biome.z = reader.read<std::int32_t>();
                biome.x = reader.read<std::int32_t>();
                biome.size = reader.read<mcp::var_int>();
                biome.data = std::vector<std::byte>(biome.size.value);
                const auto data = reader.read_n(biome.data.size());
                std::copy(data.begin(), data.end(), biome.data.begin());
                biomes.push_back(biome);
            }

            play_chunk_biomes_c::handler(base_handle, biomes);
        }
    };

    template<auto Handler>
    struct play_clear_titles_c : public detail::packet_base<0x0F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(bool reset) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(reset);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto reset = reader.read<bool>();

            play_clear_titles_c::handler(base_handle, reset);
        }
    };

    template <typename TextComponent>
    struct command_suggestion {
        std::string match;
        std::optional<TextComponent> tooltip;
    };
    template<auto Handler>
    struct play_command_suggestions_response_c : public detail::packet_base<0x10, Handler> {
        template <typename TextComponent, typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int id,
                mcp::var_int start,
                mcp::var_int length,
                std::span<command_suggestion<TextComponent>> suggestions) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(id);
            writer.write(start);
            writer.write(length);
            writer.write(mcp::var_int(suggestions.size()));
            for (const auto &suggestion : suggestions) {
                writer.write(suggestion.match);
                if (suggestion.tooltip) {
                    writer.write(true);
                    detail::get_type_t<mcp::text_component, Converters...>::to(*suggestion.tooltip, writer);
                } else {
                    writer.write(false);
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            using TextComponent = typename detail::get_type_t<mcp::text_component, Converters...>::type_target;

            const auto id = reader.read<mcp::var_int>();
            const auto start = reader.read<mcp::var_int>();
            const auto length = reader.read<mcp::var_int>();
            const auto suggestions_count = reader.read<mcp::var_int>();
            std::vector<command_suggestion<TextComponent>> suggestions;
            suggestions.reserve(suggestions_count.value);

            for (std::size_t i = 0; i < suggestions_count.value; ++i) {
                command_suggestion<TextComponent> suggestion;
                suggestion.match = reader.read<std::string>();
                const auto has_tooltip = reader.read<bool>();
                if (has_tooltip) {
                    suggestion.tooltip = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
                }
                suggestions.push_back(suggestion);
            }

            play_command_suggestions_response_c::handler(base_handle, id, start, length, suggestions);
        }
    };

    template<auto Handler>
    struct play_commands_c : public detail::packet_base<0x11, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(std::span<mcp::command_node> commands, mcp::var_int root) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(commands.size()));
            for (const auto &command : commands) {
                detail::get_type_t<mcp::command_node, Converters...>::to(command, writer);
            }
            writer.write(root);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto commands_count = reader.read<mcp::var_int>();
            std::vector<typename detail::get_type_t<mcp::command_node, Converters...>::type_target> commands;
            commands.reserve(commands_count.value);

            for (std::size_t i = 0; i < commands_count.value; ++i) {
                commands.push_back(detail::get_type_t<mcp::command_node, Converters...>::from(reader));
            }

            const auto root = reader.read<mcp::var_int>();

            play_commands_c::handler(base_handle, commands, root);
        }
    };

    template<auto Handler>
    struct play_close_container_c : public detail::packet_base<0x12, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(std::uint8_t window_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();

            play_close_container_c::handler(base_handle, window_id);
        }
    };

    template<auto Handler>
    struct play_set_container_content_c : public detail::packet_base<0x13, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t window_id,
                mcp::var_int state_id,
                std::span<typename detail::get_type_t<mcp::slot, Converters...>::type_target> slots,
                detail::get_type_t<mcp::slot, Converters...>::type_target carried_item) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(state_id);
            writer.write(mcp::var_int(slots.size()));
            for (const auto &slot : slots) {
                detail::get_type_t<mcp::slot, Converters...>::to(slot, writer);
            }
            detail::get_type_t<mcp::slot, Converters...>::to(carried_item, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();
            const auto state_id = reader.read<mcp::var_int>();
            const auto slots_count = reader.read<mcp::var_int>();
            std::vector<typename detail::get_type_t<mcp::slot, Converters...>::type_target> slots;
            slots.reserve(slots_count.value);

            for (std::size_t i = 0; i < slots_count.value; ++i) {
                slots.push_back(detail::get_type_t<mcp::slot, Converters...>::from(reader));
            }
            const auto carried_item = detail::get_type_t<mcp::slot, Converters...>::from(reader);

            play_set_container_content_c::handler(base_handle, window_id, state_id, slots, carried_item);
        }
    };

    template<auto Handler>
    struct play_set_container_property_c : public detail::packet_base<0x14, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t window_id,
                std::int16_t property,
                std::int16_t value) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(property);
            writer.write(value);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();
            const auto property = reader.read<std::int16_t>();
            const auto value = reader.read<std::int16_t>();

            play_set_container_property_c::handler(base_handle, window_id, property, value);
        }
    };

    template<auto Handler>
    struct play_set_container_slot_c : public detail::packet_base<0x15, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t window_id,
                mcp::var_int state_id,
                std::int16_t slot,
                detail::get_type_t<mcp::slot, Converters...>::type_target slot_data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(state_id);
            writer.write(slot);
            detail::get_type_t<mcp::slot, Converters...>::to(slot_data, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::int8_t>();
            const auto state_id = reader.read<mcp::var_int>();
            const auto slot = reader.read<std::int16_t>();
            const auto slot_data = detail::get_type_t<mcp::slot, Converters...>::from(reader);

            play_set_container_slot_c::handler(base_handle, window_id, state_id, slot, slot_data);
        }
    };

    template<auto Handler>
    struct play_set_cooldown_c : public detail::packet_base<0x16, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int item_id,
                mcp::var_int cooldown_ticks) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(item_id);
            writer.write(cooldown_ticks);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto item_id = reader.read<mcp::var_int>();
            const auto cooldown_ticks = reader.read<mcp::var_int>();

            play_set_cooldown_c::handler(base_handle, item_id, cooldown_ticks);
        }
    };

    template<auto Handler>
    struct play_chat_suggestions_c : public detail::packet_base<0x17, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int action,
                std::span<std::string> entries) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(action);
            writer.write(mcp::var_int(entries.size()));
            for (const auto &entry : entries) {
                writer.write(entry);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto action = reader.read<mcp::var_int>();
            const auto entries_count = reader.read<mcp::var_int>();
            std::vector<std::string> entries;
            entries.reserve(entries_count.value);

            for (std::size_t i = 0; i < entries_count.value; ++i) {
                entries.push_back(reader.read<std::string>());
            }

            play_chat_suggestions_c::handler(base_handle, action, entries);
        }
    };

    template<auto Handler>
    struct play_plugin_message_c : public detail::packet_base<0x18, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string channel,
                std::span<std::byte> data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(channel);
            writer.write(data);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto channel = reader.read<std::string>();
            const auto data = reader.remaining();

            play_plugin_message_c::handler(base_handle, channel, data);
        }
    };

    template<auto Handler>
    struct play_damage_event_c : public detail::packet_base<0x19, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                mcp::var_int source_type_id,
                mcp::var_int source_cause_id,
                mcp::var_int source_direct_id,
                std::optional<double> x,
                std::optional<double> y,
                std::optional<double> z) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(source_type_id);
            writer.write(source_cause_id);
            writer.write(source_direct_id);

            if (x.has_value() != y.has_value() || y.has_value() != z.has_value()) {
                throw std::runtime_error("Damage event must have all three of x, y, and z");
            }

            const auto has_position = x.has_value();
            writer.write(has_position);
            if (has_position) {
                writer.write(x.value());
                writer.write(y.value());
                writer.write(z.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto source_type_id = reader.read<mcp::var_int>();
            const auto source_cause_id = reader.read<mcp::var_int>();
            const auto source_direct_id = reader.read<mcp::var_int>();
            const auto has_position = reader.read<bool>();
            const auto x = has_position ? reader.read<double>() : std::optional<double>();
            const auto y = has_position ? reader.read<double>() : std::optional<double>();
            const auto z = has_position ? reader.read<double>() : std::optional<double>();

            play_damage_event_c::handler(base_handle, entity_id, source_type_id, source_cause_id, source_direct_id, x, y, z);
        }
    };

    template<auto Handler>
    struct play_delete_message_c : public detail::packet_base<0x1A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int message_id,
                std::optional<std::span<std::byte>> signature) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (message_id.value == 0 && !signature.has_value()) {
                throw std::runtime_error("Message id 0 must have a signature");
            }
            if (signature.has_value() && signature.value().size() != 256) {
                throw std::runtime_error("Signature must be 256 bytes");
            }

            writer.write(message_id);
            if (message_id.value == 0 && signature.has_value()) {
                writer.write(signature.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message_id = reader.read<mcp::var_int>();
            auto signature = std::optional<std::vector<std::byte>>();
            if (message_id.value == 0) {
                const auto stored = reader.read_n(256);
                signature = std::vector<std::byte>(stored.begin(), stored.end());
            }

            play_delete_message_c::handler(base_handle, message_id, signature);
        }
    };

    template<auto Handler>
    struct play_disconnect_c : public detail::packet_base<0x1B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target reason) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(reason, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto reason = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_disconnect_c::handler(base_handle, reason);
        }
    };

    template<auto Handler>
    struct play_disguised_chat_message_c : public detail::packet_base<0x1C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target message,
                mcp::var_int chat_type,
                detail::get_type_t<mcp::text_component, Converters...>::type_target sender_name,
                std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> target_name) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(message, writer);
            writer.write(chat_type);
            detail::get_type_t<mcp::text_component, Converters...>::to(sender_name, writer);
            writer.write(target_name.has_value());
            if (target_name.has_value()) {
                detail::get_type_t<mcp::text_component, Converters...>::to(*target_name, writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            const auto chat_type = reader.read<mcp::var_int>();
            const auto sender_name = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            const auto has_target_name = reader.read<bool>();
            std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> target_name;
            if (has_target_name) {
                target_name = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            }

            play_disguised_chat_message_c::handler(base_handle, message, chat_type, sender_name, target_name);
        }
    };

    template<auto Handler>
    struct play_entity_event_c : public detail::packet_base<0x1D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t entity_id,
                std::int8_t entity_status) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(entity_status);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<std::int32_t>();
            const auto entity_status = reader.read<std::int8_t>();

            play_entity_event_c::handler(base_handle, entity_id, entity_status);
        }
    };

    struct play_explosion_event_c_record {
        std::int8_t x;
        std::int8_t y;
        std::int8_t z;
    };
    template<auto Handler>
    struct play_explosion_c : public detail::packet_base<0x1E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x, double y, double z,
                float strength,
                std::span<play_explosion_event_c_record> records,
                float player_motion_x,
                float player_motion_y,
                float player_motion_z,
                mcp::var_int block_interaction,
                mcp::var_int small_explosion_particle_id,
                // Todo - small explosion particle data
                mcp::var_int large_explosion_particle_id,
                // Todo - large explosion particle data
                std::string sound_name,
                std::optional<float> range) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(strength);
            writer.write(mcp::var_int(records.size()));
            for (const auto &record : records) {
                writer.write(record.x);
                writer.write(record.y);
                writer.write(record.z);
            }
            writer.write(player_motion_x);
            writer.write(player_motion_y);
            writer.write(player_motion_z);
            writer.write(block_interaction);
            writer.write(small_explosion_particle_id);
            writer.write(large_explosion_particle_id);
            writer.write(sound_name);
            writer.write(range.has_value());
            if (range.has_value()) {
                writer.write(range.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto strength = reader.read<float>();
            const auto records_count = reader.read<mcp::var_int>();
            std::vector<play_explosion_event_c_record> records;
            records.reserve(records_count.value);

            for (std::size_t i = 0; i < records_count.value; ++i) {
                play_explosion_event_c_record record;
                record.x = reader.read<std::int8_t>();
                record.y = reader.read<std::int8_t>();
                record.z = reader.read<std::int8_t>();
                records.push_back(record);
            }

            const auto player_motion_x = reader.read<float>();
            const auto player_motion_y = reader.read<float>();
            const auto player_motion_z = reader.read<float>();
            const auto block_interaction = reader.read<mcp::var_int>();
            const auto small_explosion_particle_id = reader.read<mcp::var_int>();
            const auto large_explosion_particle_id = reader.read<mcp::var_int>();
            const auto sound_name = reader.read<std::string>();
            const auto has_range = reader.read<bool>();
            const auto range = has_range ? reader.read<float>() : std::optional<float>();

            play_explosion_c::handler(base_handle, x, y, z, strength, records, player_motion_x, player_motion_y, player_motion_z, block_interaction, small_explosion_particle_id, large_explosion_particle_id, sound_name, range);
        }
    };

    template<auto Handler>
    struct play_unload_chunk_c : public detail::packet_base<0x1F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t z,
                std::int32_t x) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(z);
            writer.write(x);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto z = reader.read<std::int32_t>();
            const auto x = reader.read<std::int32_t>();

            play_unload_chunk_c::handler(base_handle, z, x);
        }
    };

    template<auto Handler>
    struct play_game_event_c : public detail::packet_base<0x20, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t event,
                float value) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(event);
            writer.write(value);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto event = reader.read<std::uint8_t>();
            const auto value = reader.read<float>();

            play_game_event_c::handler(base_handle, event, value);
        }
    };

    template<auto Handler>
    struct play_open_horse_screen_c : public detail::packet_base<0x21, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t window_id,
                mcp::var_int slot_count,
                std::int32_t entity_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(slot_count);
            writer.write(entity_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();
            const auto slot_count = reader.read<mcp::var_int>();
            const auto entity_id = reader.read<std::int32_t>();

            play_open_horse_screen_c::handler(base_handle, window_id, slot_count, entity_id);
        }
    };

    template<auto Handler>
    struct play_hurt_animation_c : public detail::packet_base<0x22, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                float yaw) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(yaw);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto yaw = reader.read<float>();

            play_hurt_animation_c::handler(base_handle, entity_id, yaw);
        }
    };

    template<auto Handler>
    struct initialize_world_border : public detail::packet_base<0x23, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double z,
                double old_diameter,
                double new_diameter,
                mcp::var_long speed,
                mcp::var_int portal_teleport_boundary,
                mcp::var_int warning_blocks,
                mcp::var_int warning_time) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(z);
            writer.write(old_diameter);
            writer.write(new_diameter);
            writer.write(speed);
            writer.write(portal_teleport_boundary);
            writer.write(warning_blocks);
            writer.write(warning_time);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto z = reader.read<double>();
            const auto old_diameter = reader.read<double>();
            const auto new_diameter = reader.read<double>();
            const auto speed = reader.read<mcp::var_long>();
            const auto portal_teleport_boundary = reader.read<mcp::var_int>();
            const auto warning_blocks = reader.read<mcp::var_int>();
            const auto warning_time = reader.read<mcp::var_int>();

            initialize_world_border::handler(base_handle, x, z, old_diameter, new_diameter, speed, portal_teleport_boundary, warning_blocks, warning_time);
        }
    };

    template<auto Handler>
    struct play_keep_alive_c : public detail::packet_base<0x24, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto id = reader.read<std::int64_t>();

            play_keep_alive_c::handler(base_handle, id);
        }
    };

    // Technically speaking - they should pass in things that aren't chunk data.
    // It might be more "polite" to the end user to allow them to pass everything as parameters
    // but due to how dependent a chunk structure might be on the implementation, we will treat
    // mcp::chunk_data as a catchall for chunks and what the users need to write.
    template<auto Handler>
    struct play_chunk_data__update_light_c : public detail::packet_base<0x25, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t x,
                std::int32_t z,
                detail::get_type_t<mcp::chunk_data, Converters...>::type_target chunk) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(z);
            detail::get_type_t<mcp::chunk_data, Converters...>::to(chunk, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<std::int32_t>();
            const auto z = reader.read<std::int32_t>();
            auto chunk = detail::get_type_t<mcp::chunk_data, Converters...>::from(reader);

            play_chunk_data__update_light_c::handler(base_handle, x, z, chunk);
        }
    };

    template<auto Handler>
    struct play_world_event_c : public detail::packet_base<0x26, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t event,
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                std::int32_t data,
                bool disable_relative_volume) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(event);
            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(data);
            writer.write(disable_relative_volume);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto event = reader.read<std::int32_t>();
            const auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto data = reader.read<std::int32_t>();
            const auto disable_relative_volume = reader.read<bool>();

            play_world_event_c::handler(base_handle, event, position, data, disable_relative_volume);
        }
    };

    template<auto Handler>
    struct play_particle_c : public detail::packet_base<0x27, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int particle_id,
                bool long_distance,
                double x,
                double y,
                double z,
                float offset_x,
                float offset_y,
                float offset_z,
                float max_speed,
                std::int32_t count,
                detail::get_type_t<mcp::particle_data, Converters...>::type_target data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(particle_id);
            writer.write(long_distance);
            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(offset_x);
            writer.write(offset_y);
            writer.write(offset_z);
            writer.write(max_speed);
            writer.write(count);
            detail::get_type_t<mcp::particle_data, Converters...>::to(data, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto particle_id = reader.read<mcp::var_int>();
            const auto long_distance = reader.read<bool>();
            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto offset_x = reader.read<float>();
            const auto offset_y = reader.read<float>();
            const auto offset_z = reader.read<float>();
            const auto max_speed = reader.read<float>();
            const auto count = reader.read<std::int32_t>();
            const auto data = detail::get_type_t<mcp::particle_data, Converters...>::from(reader);

            play_particle_c::handler(base_handle, particle_id, long_distance, x, y, z, offset_x, offset_y, offset_z, max_speed, count, data);
        }
    };

    template<auto Handler>
    struct play_update_light : public detail::packet_base<0x28, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int x,
                mcp::var_int z,
                detail::get_type_t<mcp::chunk_light_data, Converters...>::type_target data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(z);
            detail::get_type_t<mcp::chunk_light_data, Converters...>::to(data, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<mcp::var_int>();
            const auto z = reader.read<mcp::var_int>();
            const auto data = detail::get_type_t<mcp::chunk_light_data, Converters...>::from(reader);

            play_update_light::handler(base_handle, x, z, data);
        }
    };

    template<auto Handler>
    struct play_login_c : public detail::packet_base<0x29, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t entity_id,
                bool is_hardcore,
                std::span<const std::string> dimensions,
                mcp::var_int max_players,
                mcp::var_int view_distance,
                mcp::var_int simulation_distance,
                bool reduced_debug_info,
                bool enable_respawn_screen,
                bool do_limited_crafting,
                std::string dimension_type,
                std::string dimension_name,
                std::int64_t hashed_seed,
                std::uint8_t game_mode,
                std::int8_t previous_game_mode,
                bool is_debug,
                bool is_flat,
                std::optional<std::string> death_dimension_name,
                std::optional<typename detail::get_type_t<mcp::position, Converters...>::type_target> death_location,
                mcp::var_int portal_cooldown) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (death_dimension_name.has_value() != death_location.has_value()) {
                throw std::runtime_error("Death dimension name and death location must be both present or both absent");
            }

            writer.write(entity_id);
            writer.write(is_hardcore);
            writer.write(mcp::var_int(dimensions.size()));
            for (const auto &dimension : dimensions) {
                writer.write(dimension);
            }
            writer.write(max_players);
            writer.write(view_distance);
            writer.write(simulation_distance);
            writer.write(reduced_debug_info);
            writer.write(enable_respawn_screen);
            writer.write(do_limited_crafting);
            writer.write(dimension_type);
            writer.write(dimension_name);
            writer.write(hashed_seed);
            writer.write(game_mode);
            writer.write(previous_game_mode);
            writer.write(is_debug);
            writer.write(is_flat);
            if (death_dimension_name.has_value()) {
                writer.write(true); // "has death location"
                writer.write(death_dimension_name.value());
            }
            if (death_location.has_value()) {
                detail::get_type_t<mcp::position, Converters...>::to(*death_location, writer);
            }
            writer.write(portal_cooldown);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<std::int32_t>();
            const auto is_hardcore = reader.read<bool>();
            const auto dimensions_count = reader.read<mcp::var_int>();
            std::vector<std::string> dimensions;
            dimensions.reserve(dimensions_count.value);

            for (std::size_t i = 0; i < dimensions_count.value; ++i) {
                dimensions.push_back(reader.read<std::string>());
            }
            const auto max_players = reader.read<mcp::var_int>();
            const auto view_distance = reader.read<mcp::var_int>();
            const auto simulation_distance = reader.read<mcp::var_int>();
            const auto reduced_debug_info = reader.read<bool>();
            const auto enable_respawn_screen = reader.read<bool>();
            const auto do_limited_crafting = reader.read<bool>();
            const auto dimension_type = reader.read<std::string>();
            const auto dimension_name = reader.read<std::string>();
            const auto hashed_seed = reader.read<std::int64_t>();
            const auto game_mode = reader.read<std::uint8_t>();
            const auto previous_game_mode = reader.read<std::int8_t>();
            const auto is_debug = reader.read<bool>();
            const auto is_flat = reader.read<bool>();
            auto death_dimension_name = std::optional<std::string>();
            auto death_location = std::optional<typename detail::get_type_t<mcp::position, Converters...>::type_target>();
            if (reader.read<bool>()) { // "has death location"
                death_dimension_name = reader.read<std::string>();
                death_location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            }
            const auto portal_cooldown = reader.read<mcp::var_int>();

        }
    };

    template <typename TextComponent>
    struct play_map_data_c_icon {
        mcp::var_int type;
        std::int8_t x;
        std::int8_t z;
        std::int8_t direction;
        std::optional<TextComponent> text;
    };
    template<auto Handler>
    struct play_map_data_c : public detail::packet_base<0x2A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int map_id,
                std::int8_t scale,
                bool locked,
                std::span<play_map_data_c_icon<typename detail::get_type_t<mcp::text_component, Converters...>::type_target>> icons,
                std::uint8_t columns,
                std::optional<std::uint8_t> rows,
                std::optional<std::uint8_t> x,
                std::optional<std::uint8_t> z,
                std::optional<std::string> name,
                std::optional<mcp::var_int> length,
                std::optional<std::span<const std::byte>> data) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (columns > 0 && (
                !rows.has_value() ||
                !x.has_value() ||
                !z.has_value() ||
                !name.has_value() ||
                !length.has_value() ||
                data.has_value())) {
                throw std::runtime_error("All map data parameters must be present");
            }

            writer.write(map_id);
            writer.write(scale);
            writer.write(locked);
            writer.write(!icons.empty());
            writer.write(mcp::var_int(icons.size()));
            for (const auto &icon : icons) {
                writer.write(icon.type);
                writer.write(icon.x);
                writer.write(icon.z);
                writer.write(icon.direction);
                writer.write(icon.text.has_value());
                if (icon.text.has_value()) {
                    detail::get_type_t<mcp::text_component, Converters...>::to(*icon.text, writer);
                }
            }
            writer.write(columns);
            if (rows.has_value()) {
                writer.write(*rows);
                writer.write(*x);
                writer.write(*z);
                writer.write(*length);
                writer.write(*data);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto map_id = reader.read<mcp::var_int>();
            const auto scale = reader.read<std::int8_t>();
            const auto locked = reader.read<bool>();
            const auto has_icons = reader.read<bool>();
            const auto icons_count = reader.read<mcp::var_int>();
            std::vector<play_map_data_c_icon<typename detail::get_type_t<mcp::text_component, Converters...>::type_target>> icons;
            icons.reserve(icons_count.value);

            for (std::size_t i = 0; i < icons_count.value; ++i) {
                play_map_data_c_icon<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> icon;
                icon.type = reader.read<mcp::var_int>();
                icon.x = reader.read<std::int8_t>();
                icon.z = reader.read<std::int8_t>();
                icon.direction = reader.read<std::int8_t>();
                const auto has_text = reader.read<bool>();
                if (has_text) {
                    icon.text = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
                }
                icons.push_back(icon);
            }

            const auto columns = reader.read<std::uint8_t>();
            auto rows = std::optional<std::uint8_t>();
            auto x = std::optional<std::int8_t>();
            auto z = std::optional<std::int8_t>();
            auto name = std::optional<std::string>();
            auto length = std::optional<mcp::var_int>();
            auto data = std::optional<std::vector<std::byte>>();
            if (has_icons) {
                rows = reader.read<std::uint8_t>();
                x = reader.read<std::int8_t>();
                z = reader.read<std::int8_t>();
                length = reader.read<mcp::var_int>();
                if (length->value > 0) {
                    data = std::vector<std::byte>(length->value);
                    const auto read_data = reader.read_n(length->value);
                    std::copy(read_data.begin(), read_data.end(), data->begin());
                }
            }

            play_map_data_c::handler(base_handle, map_id, scale, locked, icons, columns, rows, x, z, name, length, data);
        }
    };

    template <typename SlotT>
    struct play_merchant_offer_trade {
        SlotT input_item_1;
        SlotT output_item;
        SlotT input_item_2;
        bool trade_disabled;
        std::int32_t number_of_trade_uses;
        std::int32_t max_trade_uses;
        std::int32_t xp;
        std::int32_t special_price;
        float price_multiplier;
        std::int32_t demand;
    };
    template<auto Handler>
    struct play_merchant_offers_c : public detail::packet_base<0x2B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int window_id,
                std::span<const play_boss_bar_c<detail::get_type_t<mcp::slot, Converters...>::type_target>> offers,
                mcp::var_int villager_level,
                mcp::var_int experience,
                bool is_regular_villager,
                bool can_restock) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(mcp::var_int(offers.size()));
            for (const auto &offer : offers) {
                detail::get_type_t<mcp::slot, Converters...>::to(offer.input_item_1, writer);
                detail::get_type_t<mcp::slot, Converters...>::to(offer.output_item, writer);
                detail::get_type_t<mcp::slot, Converters...>::to(offer.input_item_2, writer);
                writer.write(offer.trade_disabled);
                writer.write(offer.number_of_trade_uses);
                writer.write(offer.max_trade_uses);
                writer.write(offer.xp);
                writer.write(offer.special_price);
                writer.write(offer.price_multiplier);
                writer.write(offer.demand);
            }
            writer.write(villager_level);
            writer.write(experience);
            writer.write(is_regular_villager);
            writer.write(can_restock);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<mcp::var_int>();
            const auto offers_count = reader.read<mcp::var_int>();
            std::vector<play_boss_bar_c<detail::get_type_t<mcp::slot, Converters...>::type_target>> offers;
            offers.reserve(offers_count.value);

            for (std::size_t i = 0; i < offers_count.value; ++i) {
                play_boss_bar_c<detail::get_type_t<mcp::slot, Converters...>::type_target> offer;
                detail::get_type_t<mcp::slot, Converters...>::from(reader, offer.input_item_1);
                detail::get_type_t<mcp::slot, Converters...>::from(reader, offer.output_item);
                detail::get_type_t<mcp::slot, Converters...>::from(reader, offer.input_item_2);
                offer.trade_disabled = reader.read<bool>();
                offer.number_of_trade_uses = reader.read<std::int32_t>();
                offer.max_trade_uses = reader.read<std::int32_t>();
                offer.xp = reader.read<std::int32_t>();
                offer.special_price = reader.read<std::int32_t>();
                offer.price_multiplier = reader.read<float>();
                offer.demand = reader.read<std::int32_t>();
                offers.push_back(offer);
            }
            const auto villager_level = reader.read<mcp::var_int>();
            const auto experience = reader.read<mcp::var_int>();
            const auto is_regular_villager = reader.read<bool>();
            const auto can_restock = reader.read<bool>();

            play_merchant_offers_c::handler(base_handle, window_id, offers, villager_level, experience, is_regular_villager, can_restock);
        }
    };

    template<auto Handler>
    struct play_update_entity_position_c : public detail::packet_base<0x2C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::int16_t delta_x,
                std::int16_t delta_y,
                std::int16_t delta_z,
                bool on_ground) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(delta_x);
            writer.write(delta_y);
            writer.write(delta_z);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto delta_x = reader.read<std::int16_t>();
            const auto delta_y = reader.read<std::int16_t>();
            const auto delta_z = reader.read<std::int16_t>();
            const auto on_ground = reader.read<bool>();

            play_update_entity_position_c::handler(base_handle, entity_id, delta_x, delta_y, delta_z, on_ground);
        }
    };

    template<auto Handler>
    struct play_update_entity_position_rotation_c : public detail::packet_base<0x2D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::int16_t delta_x,
                std::int16_t delta_y,
                std::int16_t delta_z,
                std::uint8_t yaw,
                std::uint8_t pitch,
                bool on_ground) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(delta_x);
            writer.write(delta_y);
            writer.write(delta_z);
            writer.write(yaw);
            writer.write(pitch);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto delta_x = reader.read<std::int16_t>();
            const auto delta_y = reader.read<std::int16_t>();
            const auto delta_z = reader.read<std::int16_t>();
            const auto yaw = reader.read<std::uint8_t>();
            const auto pitch = reader.read<std::uint8_t>();
            const auto on_ground = reader.read<bool>();

            play_update_entity_position_rotation_c::handler(base_handle, entity_id, delta_x, delta_y, delta_z, yaw, pitch, on_ground);
        }
    };

    template<auto Handler>
    struct play_update_entity_rotation_c : public detail::packet_base<0x2E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::int16_t yaw,
                std::int16_t pitch,
                bool on_ground) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(yaw);
            writer.write(pitch);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto yaw = reader.read<std::int16_t>();
            const auto pitch = reader.read<std::int16_t>();
            const auto on_ground = reader.read<bool>();

            play_update_entity_rotation_c::handler(base_handle, entity_id, yaw, pitch, on_ground);
        }
    };

    template<auto Handler>
    struct play_move_vehicle_c : public detail::packet_base<0x2F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double y,
                double z,
                std::int8_t yaw,
                std::int8_t pitch) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(yaw);
            writer.write(pitch);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto yaw = reader.read<std::int8_t>();
            const auto pitch = reader.read<std::int8_t>();

            play_move_vehicle_c::handler(base_handle, x, y, z, yaw, pitch);
        }
    };

    template<auto Handler>
    struct play_open_book_c : public detail::packet_base<0x30, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int hand) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(hand);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto hand = reader.read<mcp::var_int>();

            play_open_book_c::handler(base_handle, hand);
        }
    };

    template<auto Handler>
    struct play_open_screen_c : public detail::packet_base<0x31, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int window_id,
                mcp::var_int window_type,
                detail::get_type_t<mcp::text_component, Converters...>::type_target title) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(window_type);
            detail::get_type_t<mcp::text_component, Converters...>::to(title, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<mcp::var_int>();
            const auto window_type = reader.read<mcp::var_int>();
            const auto title = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_open_screen_c::handler(base_handle, window_id, window_type, title);
        }
    };

    template<auto Handler>
    struct play_open_sign_editor_c : public detail::packet_base<0x32, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                bool is_front_text) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(is_front_text);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto is_front_text = reader.read<bool>();

            play_open_sign_editor_c::handler(base_handle, position, is_front_text);
        }
    };

    template<auto Handler>
    struct play_ping_c : public detail::packet_base<0x33, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto id = reader.read<std::int32_t>();

            play_ping_c::handler(base_handle, id);
        }
    };

    template<auto Handler>
    struct ping_response_c : public detail::packet_base<0x34, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t payload) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(payload);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto payload = reader.read<std::int64_t>();

            ping_response_c::handler(base_handle, payload);
        }
    };

    template<auto Handler>
    struct place_ghost_recipe_c : public detail::packet_base<0x35, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t window_id,
                std::string recipe_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(recipe_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::int8_t>();
            const auto recipe_id = reader.read<std::string>();

            place_ghost_recipe_c::handler(base_handle, window_id, recipe_id);
        }
    };

    template<auto Handler>
    struct play_player_abilities_c : public detail::packet_base<0x36, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t flags,
                float fly_speed,
                float fov_modifier) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(flags);
            writer.write(fly_speed);
            writer.write(fov_modifier);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto flags = reader.read<std::int8_t>();
            const auto fly_speed = reader.read<float>();
            const auto fov_modifier = reader.read<float>();

            play_player_abilities_c::handler(base_handle, flags, fly_speed, fov_modifier);
        }
    };

    template<auto Handler>
    struct play_player_chat_message_c : public detail::packet_base<0x37, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                mcp::var_int index,
                std::span<const std::byte> signature,
                std::string message,
                std::int64_t timestamp,
                std::int64_t salt) {
//            auto buffer = std::vector<std::byte>();
//            auto writer = mcp::writer(buffer);


            throw std::runtime_error("Not implemented, use disguised chat packet instead.");
//            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

        }
    };

    template<auto Handler>
    struct play_end_combat_c : public detail::packet_base<0x38, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int duration) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(duration);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto duration = reader.read<mcp::var_int>();

            play_end_combat_c::handler(base_handle, duration);
        }
    };

    template<auto Handler>
    struct play_enter_combat_c : public detail::packet_base<0x39, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            play_enter_combat_c::handler(base_handle);
        }
    };

    template<auto Handler>
    struct play_combat_death_c : public detail::packet_base<0x3A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int player_id,
                detail::get_type_t<mcp::text_component, Converters...>::type_target message) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(player_id);
            detail::get_type_t<mcp::text_component, Converters...>::to(message, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto player_id = reader.read<mcp::var_int>();
            const auto message = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_combat_death_c::handler(base_handle, player_id, message);
        }
    };

    template<auto Handler>
    struct play_player_info_remove_c : public detail::packet_base<0x3B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> uuids){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(uuids.size()));
            for (const auto &uuid : uuids) {
                detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

        }
    };

    template <typename UUID, typename TextComponent>
    struct player_info_update_action {
        struct add_t {
            bool active = false;
            std::string name;
            std::string value;
            std::optional<std::string> signature;
        } add;
        struct initialize_chat_t {
            bool active = false;
            bool has_signature_data = false;
            UUID chat_session;
            std::int64_t public_key_expiry_time;
            mcp::var_int public_key_size;
            std::vector<std::byte> encoded_public_key;
            std::vector<std::byte> signature;
        } initialize_chat;
        struct update_game_mode_t {
            bool active = false;
            mcp::var_int game_mode;
        } update_game_mode;
        struct listed_t {
            bool active = false;
            bool is_listed;
        } listed;
        struct latency_t {
            bool active = false;
            mcp::var_int latency;
        } latency;
        struct display_name_t {
            bool active = false;
            std::optional<TextComponent> display_name;
        } display_name;
    };

    template<auto Handler>
    struct play_player_info_update_c : public detail::packet_base<0x3C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<player_info_update_action<
                        typename detail::get_type_t<mcp::uuid, Converters...>::type_target,
                        typename detail::get_type_t<mcp::text_component, Converters...>::type_target>> entries
                ) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            auto action_flag = 0x00;
            for (const auto &entry : entries) {
                const auto entry_flag =
                        (entry.add.active ? 0x01 : 0x00) +
                        (entry.initialize_chat.active ? 0x02 : 0x00) +
                        (entry.update_game_mode.active ? 0x04 : 0x00) +
                        (entry.listed.active ? 0x08 : 0x00) +
                        (entry.latency.active ? 0x10 : 0x00) +
                        (entry.display_name.active ? 0x20 : 0x00);

                if (action_flag == 0x00) {
                    action_flag = entry_flag;
                }

                if (action_flag != entry_flag) {
                    throw std::runtime_error("All player info update actions must be present");
                }
            }

            writer.write(mcp::var_int(action_flag));
            writer.write(mcp::var_int(entries.size()));
            for (const auto &entry : entries) {
                if (entry.add.active) {
                    writer.write(entry.add.name);
                    writer.write(entry.add.value);
                    writer.write(entry.add.signature.has_value());
                    if (entry.add.signature.has_value()) {
                        writer.write(entry.add.signature.value());
                    }
                }
                if (entry.initialize_chat.active) {
                    writer.write(entry.initialize_chat.chat_session);
                    writer.write(entry.initialize_chat.public_key_expiry_time);
                    writer.write(entry.initialize_chat.public_key_size);
                    writer.write(entry.initialize_chat.encoded_public_key);
                    writer.write(entry.initialize_chat.signature);
                }
                if (entry.update_game_mode.active) {
                    writer.write(entry.update_game_mode.game_mode);
                }
                if (entry.listed.active) {
                    writer.write(entry.listed.is_listed);
                }
                if (entry.latency.active) {
                    writer.write(entry.latency.latency);
                }
                if (entry.display_name.active) {
                    writer.write(entry.display_name.display_name.has_value());
                    if (entry.display_name.display_name.has_value()) {
                        detail::get_type_t<mcp::text_component, Converters...>::to(*entry.display_name.display_name, writer);
                    }
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto action_flag = reader.read<mcp::var_int>().value;
            const auto entries_count = reader.read<mcp::var_int>().value;
            std::vector<player_info_update_action<
                    typename detail::get_type_t<mcp::uuid, Converters...>::type_target,
                    typename detail::get_type_t<mcp::text_component, Converters...>::type_target>> entries;
            entries.reserve(entries_count);

            for (std::size_t i = 0; i < entries_count; ++i) {
                auto entry = player_info_update_action<
                        typename detail::get_type_t<mcp::uuid, Converters...>::type_target,
                        typename detail::get_type_t<mcp::text_component, Converters...>::type_target>();

                if (action_flag & 0x01) {
                    entry.add.active = true;
                    entry.add.name = reader.read<std::string>();
                    entry.add.value = reader.read<std::string>();
                    entry.add.signature.has_value() = reader.read<bool>();
                    if (entry.add.signature.has_value()) {
                        entry.add.signature.value() = reader.read<std::string>();
                    }
                }
                if (action_flag & 0x02) {
                    entry.initialize_chat.active = true;
                    entry.initialize_chat.chat_session = reader.read<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>();
                    entry.initialize_chat.public_key_expiry_time = reader.read<mcp::var_int>();
                    entry.initialize_chat.public_key_size = reader.read<mcp::var_int>();
                    entry.initialize_chat.encoded_public_key = reader.read_n(entry.initialize_chat.public_key_size.value);
                    entry.initialize_chat.signature = reader.read_n(256);
                }
                if (action_flag & 0x04) {
                    entry.update_game_mode.active = true;
                    entry.update_game_mode.game_mode = reader.read<mcp::var_int>();
                }
                if (action_flag & 0x08) {
                    entry.listed.active = true;
                    entry.listed.is_listed = reader.read<bool>();
                }
                if (action_flag & 0x10) {
                    entry.latency.active = true;
                    entry.latency.latency = reader.read<mcp::var_int>();
                }
                if (action_flag & 0x20) {
                    entry.display_name.active = true;
                    entry.display_name.display_name.has_value() = reader.read<bool>();
                    if (entry.display_name.display_name.has_value()) {
                        detail::get_type_t<mcp::text_component, Converters...>::from(reader, entry.display_name.display_name.value());
                    }
                }
            }

            play_player_info_update_c::handler(base_handle, entries);
        }
    };

    template<auto Handler>
    struct play_look_at_c : public detail::packet_base<0x3D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int feet_eyes,
                double target_x,
                double target_y,
                double target_z,
                std::optional<mcp::var_int> entity_id,
                std::optional<mcp::var_int> entity_feet_eyes){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (entity_id.has_value() != entity_feet_eyes.has_value()) {
                throw std::runtime_error("Entity ID and entity feet eyes must be both present or both absent");
            }

            writer.write(feet_eyes);
            writer.write(target_x);
            writer.write(target_y);
            writer.write(target_z);
            writer.write(entity_id.has_value());
            if (entity_id.has_value()) {
                writer.write(entity_id.value());
                writer.write(entity_feet_eyes.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto feet_eyes = reader.read<mcp::var_int>();
            const auto target_x = reader.read<double>();
            const auto target_y = reader.read<double>();
            const auto target_z = reader.read<double>();
            auto entity_id = std::optional<mcp::var_int>();
            auto entity_feet_eyes = std::optional<mcp::var_int>();
            if (reader.read<bool>()) {
                entity_id = reader.read<mcp::var_int>();
                entity_feet_eyes = reader.read<mcp::var_int>();
            }

            play_look_at_c::handler(base_handle, feet_eyes, target_x, target_y, target_z, entity_id, entity_feet_eyes);
        }
    };

    template<auto Handler>
    struct play_synchronize_player_position_c : public detail::packet_base<0x3E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double y,
                double z,
                float yaw,
                float pitch,
                std::int8_t flags,
                mcp::var_int teleport_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(yaw);
            writer.write(pitch);
            writer.write(flags);
            writer.write(teleport_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto yaw = reader.read<float>();
            const auto pitch = reader.read<float>();
            const auto flags = reader.read<std::int8_t>();
            const auto teleport_id = reader.read<mcp::var_int>();

            play_synchronize_player_position_c::handler(base_handle, x, y, z, yaw, pitch, flags, teleport_id);
        }
    };

    template<auto Handler>
    struct play_update_recipe_book_c : public detail::packet_base<0x3F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int action,
                bool crafting_recipe_book_open,
                bool crafting_recipe_book_filter_active,
                bool smelting_recipebook_open,
                bool smelting_recipe_book_filter_active,
                bool blast_furnace_recipe_book_open,
                bool blast_furnace_recipe_book_filter_active,
                bool smoker_recipe_book_open,
                bool smoker_recipe_book_filter_active,
                std::span<const std::string> recipe_ids1,
                std::span<const std::string> recipe_ids2){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(action);
            writer.write(crafting_recipe_book_open);
            writer.write(crafting_recipe_book_filter_active);
            writer.write(smelting_recipebook_open);
            writer.write(smelting_recipe_book_filter_active);
            writer.write(blast_furnace_recipe_book_open);
            writer.write(blast_furnace_recipe_book_filter_active);
            writer.write(smoker_recipe_book_open);
            writer.write(smoker_recipe_book_filter_active);
            writer.write(mcp::var_int(recipe_ids1.size()));
            for (const auto &recipe_id : recipe_ids1) {
                writer.write(recipe_id);
            }
            if (action.value == 0) {
                writer.write(mcp::var_int(recipe_ids2.size()));
                for (const auto &recipe_id : recipe_ids2) {
                    writer.write(recipe_id);
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto action = reader.read<mcp::var_int>();
            const auto crafting_recipe_book_open = reader.read<bool>();
            const auto crafting_recipe_book_filter_active = reader.read<bool>();
            const auto smelting_recipebook_open = reader.read<bool>();
            const auto smelting_recipe_book_filter_active = reader.read<bool>();
            const auto blast_furnace_recipe_book_open = reader.read<bool>();
            const auto blast_furnace_recipe_book_filter_active = reader.read<bool>();
            const auto smoker_recipe_book_open = reader.read<bool>();
            const auto smoker_recipe_book_filter_active = reader.read<bool>();
            const auto recipe_ids1_count = reader.read<mcp::var_int>();
            std::vector<std::string> recipe_ids1;
            recipe_ids1.reserve(recipe_ids1_count.value);
            for (std::size_t i = 0; i < recipe_ids1_count.value; ++i) {
                recipe_ids1.push_back(reader.read<std::string>());
            }
            std::vector<std::string> recipe_ids2;
            if (action.value == 0) {
                const auto recipe_ids2_count = reader.read<mcp::var_int>();
                recipe_ids2.reserve(recipe_ids2_count.value);
                for (std::size_t i = 0; i < recipe_ids2_count.value; ++i) {
                    recipe_ids2.push_back(reader.read<std::string>());
                }
            }

            play_update_recipe_book_c::handler(base_handle,
                                               action,
                                               crafting_recipe_book_open, crafting_recipe_book_filter_active,
                                               smelting_recipebook_open, smelting_recipe_book_filter_active,
                                               blast_furnace_recipe_book_open, blast_furnace_recipe_book_filter_active,
                                               smoker_recipe_book_open, smoker_recipe_book_filter_active,
                                               recipe_ids1, recipe_ids2);
        }
    };

    template<auto Handler>
    struct play_remove_entities_c : public detail::packet_base<0x40, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<mcp::var_int> entity_ids) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(entity_ids.size()));
            for (const auto &entity_id : entity_ids) {
                writer.write(entity_id);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id_count = reader.read<mcp::var_int>();
            std::vector<mcp::var_int> entity_ids;
            entity_ids.reserve(entity_id_count.value);
            for (std::size_t i = 0; i < entity_id_count.value; ++i) {
                entity_ids.push_back(reader.read<mcp::var_int>());
            }

            play_remove_entities_c::handler(base_handle, entity_ids);
        }
    };

    template<auto Handler>
    struct play_remove_entity_effect_c : public detail::packet_base<0x41, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                mcp::var_int effect_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(effect_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto effect_id = reader.read<mcp::var_int>();

            play_remove_entity_effect_c::handler(base_handle, entity_id, effect_id);
        }
    };

    template<auto Handler>
    struct play_reset_score_c : public detail::packet_base<0x42, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string entity_name,
                std::optional<std::string> objective_name){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_name);
            writer.write(objective_name.has_value());
            if (objective_name.has_value()) {
                writer.write(objective_name.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_name = reader.read<std::string>();
            auto objective_name = std::optional<std::string>();
            if (reader.read<bool>()) {
                objective_name = reader.read<std::string>();
            }

            play_reset_score_c::handler(base_handle, entity_name, objective_name);
        }
    };

    template<auto Handler>
    struct play_remove_resource_pack_c : public detail::packet_base<0x43, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::optional<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> resource_pack_uuid){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(resource_pack_uuid.has_value());
            if (resource_pack_uuid.has_value()) {
                writer.write(resource_pack_uuid.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto resource_pack_uuid = std::optional<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>();
            if (reader.read<bool>()) {
                resource_pack_uuid = reader.read<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>();
            }

            play_remove_resource_pack_c::handler(base_handle, resource_pack_uuid);
        }
    };

    template<auto Handler>
    struct play_add_resource_pack_c : public detail::packet_base<0x44, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target resource_pack_uuid,
                std::string url,
                std::string hash,
                bool forced,
                std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> prompt
        ){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(resource_pack_uuid, writer);
            writer.write(url);
            writer.write(hash);
            writer.write(forced);
            writer.write(prompt.has_value());
            if (prompt.has_value()) {
                detail::get_type_t<mcp::text_component, Converters...>::to(*prompt, writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto resource_pack_uuid = reader.read<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>();
            const auto url = reader.read<std::string>();
            const auto hash = reader.read<std::string>();
            const auto forced = reader.read<bool>();
            auto prompt = std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target>();
            if (reader.read<bool>()) {
                prompt = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            }

            play_add_resource_pack_c::handler(base_handle, resource_pack_uuid, url, hash, forced, prompt);
        }
    };

    template<auto Handler>
    struct play_respawn_c : public detail::packet_base<0x45, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string dimension_type,
                std::string dimension_name,
                std::int64_t hashed_seed,
                std::uint8_t game_mode,
                std::int8_t previous_game_mode,
                bool is_debug,
                bool is_flat,
                std::optional<std::string> death_dimension_name,
                std::optional<typename detail::get_type_t<mcp::position, Converters...>::type_target> death_location,
                mcp::var_int portal_cooldown,
                std::int8_t data_kept){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (death_dimension_name.has_value() != death_location.has_value()) {
                throw std::runtime_error("Death dimension name and death location must be both present or both absent");
            }

            writer.write(dimension_type);
            writer.write(dimension_name);
            writer.write(hashed_seed);
            writer.write(game_mode);
            writer.write(previous_game_mode);
            writer.write(is_debug);
            writer.write(is_flat);
            writer.write(death_dimension_name.has_value());
            if (death_dimension_name.has_value()) {
                writer.write(death_dimension_name.value());
                detail::get_type_t<mcp::position, Converters...>::to(*death_location, writer);
            }
            writer.write(portal_cooldown);
            writer.write(data_kept);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto dimension_type = reader.read<std::string>();
            const auto dimension_name = reader.read<std::string>();
            const auto hashed_seed = reader.read<std::int64_t>();
            const auto game_mode = reader.read<std::uint8_t>();
            const auto previous_game_mode = reader.read<std::int8_t>();
            const auto is_debug = reader.read<bool>();
            const auto is_flat = reader.read<bool>();
            auto death_dimension_name = std::optional<std::string>();
            auto death_location = std::optional<typename detail::get_type_t<mcp::position, Converters...>::type_target>();
            if (reader.read<bool>()) { // "has death location"
                death_dimension_name = reader.read<std::string>();
                death_location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            }
            const auto portal_cooldown = reader.read<mcp::var_int>();
            const auto data_kept = reader.read<std::int8_t>();

            play_respawn_c::handler(base_handle,
                                    dimension_type, dimension_name,
                                    hashed_seed,
                                    game_mode, previous_game_mode,
                                    is_debug, is_flat,
                                    death_dimension_name, death_location,
                                    portal_cooldown,
                                    data_kept);
        }
    };

    template<auto Handler>
    struct play_set_head_rotation_c : public detail::packet_base<0x46, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::uint8_t head_yaw){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(head_yaw);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto head_yaw = reader.read<std::uint8_t>();

            play_set_head_rotation_c::handler(base_handle, entity_id, head_yaw);
        }
    };

    template<auto Handler>
    struct play_update_section_blocks_c : public detail::packet_base<0x47, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t chunk_section_position,
                std::span<const mcp::var_long> blocks
        ){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(chunk_section_position);
            writer.write(mcp::var_int(blocks.size()));
            for (const auto &block : blocks) {
                writer.write(block);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto chunk_section_position = reader.read<std::int64_t>();
            const auto blocks_count = reader.read<mcp::var_int>();
            std::vector<mcp::var_long> blocks;
            blocks.reserve(blocks_count.value);
            for (std::size_t i = 0; i < blocks_count.value; ++i) {
                blocks.push_back(reader.read<mcp::var_long>());
            }

            play_update_section_blocks_c::handler(base_handle, chunk_section_position, blocks);
        }
    };

    template<auto Handler>
    struct play_select_advancements_tab_c : public detail::packet_base<0x48, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::optional<std::string> identifier){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(identifier.has_value());
            if (identifier.has_value()) {
                writer.write(identifier.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto identifier = std::optional<std::string>();
            if (reader.read<bool>()) {
                identifier = reader.read<std::string>();
            }

            play_select_advancements_tab_c::handler(base_handle, identifier);
        }
    };

    template<auto Handler>
    struct play_server_data_c : public detail::packet_base<0x49, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target motd,
                std::span<const std::byte> favicon,
                bool enforces_secure_chat){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(motd, writer);
            writer.write(!favicon.empty());
            writer.write(favicon.size());
            writer.write(favicon);
            writer.write(enforces_secure_chat);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto motd = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            const auto has_favicon = reader.read<bool>();
            const auto favicon_size = reader.read<mcp::var_int>();
            const auto favicon = reader.read_n(favicon_size.value);
            const auto enforces_secure_chat = reader.read<bool>();

            play_server_data_c::handler(base_handle, motd, favicon, enforces_secure_chat);
        }
    };

    template<auto Handler>
    struct play_set_action_bar_text_c : public detail::packet_base<0x4A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target text){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(text, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto text = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_set_action_bar_text_c::handler(base_handle, text);
        }
    };

    template<auto Handler>
    struct play_set_border_center_c : public detail::packet_base<0x4B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double z){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(z);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto z = reader.read<double>();

            play_set_border_center_c::handler(base_handle, x, z);
        }
    };

    template<auto Handler>
    struct play_set_border_lerp_size_c : public detail::packet_base<0x4C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double old_diameter,
                double new_diameter,
                mcp::var_long speed){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(old_diameter);
            writer.write(new_diameter);
            writer.write(speed);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto old_diameter = reader.read<double>();
            const auto new_diameter = reader.read<double>();
            const auto speed = reader.read<mcp::var_long>();

            play_set_border_lerp_size_c::handler(base_handle, old_diameter, new_diameter, speed);
        }
    };

    template<auto Handler>
    struct play_set_border_size_c : public detail::packet_base<0x4D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double diameter){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(diameter);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto diameter = reader.read<double>();

            play_set_border_size_c::handler(base_handle, diameter);
        }
    };

    template<auto Handler>
    struct play_set_border_warning_delay_c : public detail::packet_base<0x4E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int warning_time){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(warning_time);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto warning_time = reader.read<mcp::var_int>();

            play_set_border_warning_delay_c::handler(base_handle, warning_time);
        }
    };

    template<auto Handler>
    struct play_set_border_warning_distance_c : public detail::packet_base<0x4F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int warning_distance){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(warning_distance);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto warning_distance = reader.read<mcp::var_int>();

            play_set_border_warning_distance_c::handler(base_handle, warning_distance);
        }
    };

    template<auto Handler>
    struct play_set_camera_c : public detail::packet_base<0x50, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int camera_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(camera_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto camera_id = reader.read<mcp::var_int>();

            play_set_camera_c::handler(base_handle, camera_id);
        }
    };

    template<auto Handler>
    struct play_set_held_item_c : public detail::packet_base<0x51, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t slot_index){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot_index);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot_index = reader.read<std::int8_t>();

            play_set_held_item_c::handler(base_handle, slot_index);
        }
    };

    template<auto Handler>
    struct play_set_center_chunk_c : public detail::packet_base<0x52, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int chunk_x,
                mcp::var_int chunk_z){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(chunk_x);
            writer.write(chunk_z);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto chunk_x = reader.read<mcp::var_int>();
            const auto chunk_z = reader.read<mcp::var_int>();

            play_set_center_chunk_c::handler(base_handle, chunk_x, chunk_z);
        }
    };

    template<auto Handler>
    struct play_set_render_distance_c : public detail::packet_base<0x53, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int render_distance){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(render_distance);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto view_distance = reader.read<mcp::var_int>();

            play_set_render_distance_c::handler(base_handle, view_distance);
        }
    };

    template<auto Handler>
    struct play_set_default_spawn_position_c : public detail::packet_base<0x54, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                float angle){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(angle);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto angle = reader.read<float>();

            play_set_default_spawn_position_c::handler(base_handle, position, angle);
        }
    };

    template<auto Handler>
    struct play_display_objective_c : public detail::packet_base<0x55, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int position,
                std::string score_name){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(position);
            writer.write(score_name);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto position = reader.read<mcp::var_int>();
            const auto score_name = reader.read<std::string>();

            play_display_objective_c::handler(base_handle, position, score_name);
        }
    };

    template<auto Handler>
    struct play_set_entity_metadata_c : public detail::packet_base<0x56, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                detail::get_type_t<mcp::entity_metadata, Converters...>::type_target metadata){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            detail::get_type_t<mcp::entity_metadata, Converters...>::to(metadata, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto metadata = detail::get_type_t<mcp::entity_metadata, Converters...>::from(reader);

            play_set_entity_metadata_c::handler(base_handle, entity_id, metadata);
        }
    };

    template<auto Handler>
    struct play_link_entities_c : public detail::packet_base<0x57, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t attached,
                std::int32_t holding){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(attached);
            writer.write(holding);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto attached = reader.read<std::int32_t>();
            const auto holding = reader.read<std::int32_t>();

            play_link_entities_c::handler(base_handle, attached, holding);
        }
    };

    template<auto Handler>
    struct play_set_entity_velocity_c : public detail::packet_base<0x58, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::int16_t velocity_x,
                std::int16_t velocity_y,
                std::int16_t velocity_z){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(velocity_x);
            writer.write(velocity_y);
            writer.write(velocity_z);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto velocity_x = reader.read<std::int16_t>();
            const auto velocity_y = reader.read<std::int16_t>();
            const auto velocity_z = reader.read<std::int16_t>();

            play_set_entity_velocity_c::handler(base_handle, entity_id, velocity_x, velocity_y, velocity_z);
        }
    };

    template <typename SlotT>
    struct play_set_equipment_c_equipment {
        std::uint8_t slot;
        SlotT item;
    };
    template<auto Handler>
    struct play_set_equipment_c : public detail::packet_base<0x59, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::span<play_set_equipment_c_equipment<typename detail::get_type_t<mcp::slot, Converters...>::type_target>> equipment){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(mcp::var_int(equipment.size()));
            for (size_t i = 0; i < equipment.size(); ++i) {
                const auto is_last = i == equipment.size() - 1;
                auto slot = equipment[i].slot;
                if (is_last) {
                    slot |= 0x80; // Last bit set
                }
                writer.write(slot);
                detail::get_type_t<mcp::slot, Converters...>::to(equipment.item, writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            std::vector<play_set_equipment_c_equipment<typename detail::get_type_t<mcp::slot, Converters...>::type_target>> equipment;
            equipment.reserve(reader.read<mcp::var_int>().value);
            for (std::size_t i = 0; i < equipment.size(); ++i) {
                auto slot = reader.read<std::uint8_t>();
                slot &= 0x7F; // Last bit set
                const auto item = detail::get_type_t<mcp::slot, Converters...>::from(reader);
                equipment.push_back({slot, item});
            }

            play_set_equipment_c::handler(base_handle, entity_id, equipment);
        }
    };

    template<auto Handler>
    struct play_set_experience_c : public detail::packet_base<0x5A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float experience_bar,
                mcp::var_int level,
                mcp::var_int total_experience){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(experience_bar);
            writer.write(level);
            writer.write(total_experience);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto experience_bar = reader.read<float>();
            const auto level = reader.read<mcp::var_int>();
            const auto total_experience = reader.read<mcp::var_int>();

            play_set_experience_c::handler(base_handle, experience_bar, level, total_experience);
        }
    };

    template<auto Handler>
    struct play_set_health_c : public detail::packet_base<0x5B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float health,
                mcp::var_int food,
                float food_saturation){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(health);
            writer.write(food);
            writer.write(food_saturation);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto health = reader.read<float>();
            const auto food = reader.read<mcp::var_int>();
            const auto food_saturation = reader.read<float>();

            play_set_health_c::handler(base_handle, health, food, food_saturation);
        }
    };

    template<auto Handler>
    struct play_update_objectives_c : public detail::packet_base<0x5C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string objective_name,
                std::int8_t mode,
                std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> objective_value,
                std::optional<mcp::var_int> type,
                std::optional<mcp::var_int> number_format){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (mode == 1) {
                if (objective_value.has_value() != type.has_value()) {
                    throw std::runtime_error("Objective value and type must be both present or both absent");
                }
            }

            writer.write(objective_name);
            writer.write(mode);
            if (objective_value.has_value()) {
                detail::get_type_t<mcp::text_component, Converters...>::to(*objective_value, writer);
            }
            if (type.has_value()) {
                writer.write(*type);
            }
            writer.write(number_format.has_value());
            if (number_format.has_value()) {
                writer.write(*number_format);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto objective_name = reader.read<std::string>();
            const auto mode = reader.read<std::int8_t>();
            auto objective_value = std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target>();
            auto type = std::optional<mcp::var_int>();
            auto number_format = std::optional<mcp::var_int>();
            if (mode != 1) {
                objective_value = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
                type = reader.read<mcp::var_int>();
                if (reader.read<bool>()) {
                    number_format = reader.read<mcp::var_int>();
                }
            }

            play_update_objectives_c::handler(base_handle, objective_name, mode, objective_value, type, number_format);
        }
    };

    template<auto Handler>
    struct play_set_passengers_c : public detail::packet_base<0x5D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int vehicle_id,
                std::span<const mcp::var_int> passengers){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(vehicle_id);
            writer.write(mcp::var_int(passengers.size()));
            for (const auto &passenger : passengers) {
                writer.write(passenger);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto vehicle_id = reader.read<mcp::var_int>();
            auto passengers = std::vector<mcp::var_int>();
            passengers.reserve(reader.read<mcp::var_int>().value);
            for (std::size_t i = 0; i < passengers.size(); ++i) {
                passengers.push_back(reader.read<mcp::var_int>());
            }

            play_set_passengers_c::handler(base_handle, vehicle_id, passengers);
        }
    };

    enum class play_update_teams_list_update {
        add,
        remove
    };
    template<auto Handler>
    struct play_update_teams_c : public detail::packet_base<0x5E, Handler> {
        // Create team
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string team_name,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_display_name,
                std::int8_t friendly_flags,
                std::string name_tag_visibility,
                std::string collision_rule,
                mcp::var_int team_color,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_prefix,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_suffix,
                std::span<std::string> entities){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(team_name);
            writer.write(std::int8_t(0)); /// Create team
            detail::get_type_t<mcp::text_component, Converters...>::to(team_display_name, writer);
            writer.write(friendly_flags);
            writer.write(name_tag_visibility);
            writer.write(collision_rule);
            writer.write(team_color);
            detail::get_type_t<mcp::text_component, Converters...>::to(team_prefix, writer);
            detail::get_type_t<mcp::text_component, Converters...>::to(team_suffix, writer);
            writer.write(mcp::var_int(entities.size()));
            for (const auto &entity : entities) {
                writer.write(entity);
            }

            return buffer;
        }

        // Remove team
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string team_name){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(team_name);
            writer.write(std::int8_t(1)); /// Remove team

            return buffer;
        }

        // Update team info
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string team_name,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_display_name,
                std::int8_t friendly_flags,
                std::string name_tag_visibility,
                std::string collision_rule,
                mcp::var_int team_color,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_prefix,
                detail::get_type_t<mcp::text_component, Converters...>::type_target team_suffix){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(team_name);
            writer.write(std::int8_t(2)); /// Update team info
            detail::get_type_t<mcp::text_component, Converters...>::to(team_display_name, writer);
            writer.write(friendly_flags);
            writer.write(name_tag_visibility);
            writer.write(collision_rule);
            writer.write(team_color);
            detail::get_type_t<mcp::text_component, Converters...>::to(team_prefix, writer);
            detail::get_type_t<mcp::text_component, Converters...>::to(team_suffix, writer);

            return buffer;
        }

        // Update entity list for team
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string team_name,
                play_update_teams_list_update update,
                std::span<const std::string> entities){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(team_name);
            if (update == play_update_teams_list_update::add) {
                writer.write(std::int8_t(3)); /// Add entities to team
            } else if (update == play_update_teams_list_update::remove) {
                writer.write(std::int8_t(4)); /// Remove entities from team
            }
            writer.write(mcp::var_int(entities.size()));
            for (const auto &entity : entities) {
                writer.write(entity);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

        }
    };

    template<auto Handler>
    struct play_update_score_c : public detail::packet_base<0x5F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string entity_name,
                std::string objective_name,
                mcp::var_int value,
                std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target> display_name,
                std::optional<mcp::var_int> number_format){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_name);
            writer.write(objective_name);
            writer.write(value);
            writer.write(display_name.has_value());
            if (display_name.has_value()) {
                detail::get_type_t<mcp::text_component, Converters...>::to(*display_name, writer);
            }
            writer.write(number_format.has_value());
            if (number_format.has_value()) {
                writer.write(*number_format);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_name = reader.read<std::string>();
            const auto objective_name = reader.read<std::string>();
            const auto value = reader.read<mcp::var_int>();
            auto display_name = std::optional<typename detail::get_type_t<mcp::text_component, Converters...>::type_target>();
            auto number_format = std::optional<mcp::var_int>();
            if (reader.read<bool>()) {
                display_name = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            }
            if (reader.read<bool>()) {
                number_format = reader.read<mcp::var_int>();
            }

            play_update_score_c::handler(base_handle, entity_name, objective_name, value, display_name, number_format);
        }
    };

    template<auto Handler>
    struct play_set_simulation_distance_c : public detail::packet_base<0x60, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int simulation_distance){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(simulation_distance);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto simulation_distance = reader.read<mcp::var_int>();

            play_set_simulation_distance_c::handler(base_handle, simulation_distance);
        }
    };

    template<auto Handler>
    struct play_set_subtitle_text_c : public detail::packet_base<0x61, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target text){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(text, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto text = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_set_subtitle_text_c::handler(base_handle, text);
        }
    };

    template<auto Handler>
    struct play_update_time_c : public detail::packet_base<0x62, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t world_age,
                std::int64_t time){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(world_age);
            writer.write(time);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto world_age = reader.read<std::int64_t>();
            const auto time = reader.read<std::int64_t>();

            play_update_time_c::handler(base_handle, world_age, time);
        }
    };

    template<auto Handler>
    struct play_set_title_text_c : public detail::packet_base<0x63, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target text){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(text, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto text = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_set_title_text_c::handler(base_handle, text);
        }
    };

    template<auto Handler>
    struct play_set_title_animation_times_c : public detail::packet_base<0x64, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t in,
                std::int32_t stay,
                std::int32_t fade_out){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(in);
            writer.write(stay);
            writer.write(fade_out);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto in = reader.read<std::int32_t>();
            const auto stay = reader.read<std::int32_t>();
            const auto fade_out = reader.read<std::int32_t>();

            play_set_title_animation_times_c::handler(base_handle, in, stay, fade_out);
        }
    };

    template<auto Handler>
    struct play_entity_sound_effect_c : public detail::packet_base<0x65, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int sound_id,
                std::optional<std::string> sound_name,
                std::optional<bool> has_fixed_range,
                std::optional<float> range,
                mcp::var_int category,
                mcp::var_int entity_id,
                float volume,
                float pitch,
                std::int64_t seed){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            if (sound_id.value != 0 && sound_name.has_value() && has_fixed_range.has_value()) {
                throw std::runtime_error("Sound parameters can't be used for sound id != 0");
            }

            writer.write(sound_id);
            if (sound_id.value == 0) {
                writer.write(sound_name.value());
                writer.write(has_fixed_range.value());
                if (has_fixed_range.value()) {
                    writer.write(range.value());
                }
            }
            writer.write(category);
            writer.write(entity_id);
            writer.write(volume);
            writer.write(pitch);
            writer.write(seed);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto sound_id = reader.read<mcp::var_int>();
            auto sound_name = std::optional<std::string>();
            auto range = std::optional<float>();
            if (sound_id.value != 0) {
                sound_name = reader.read<std::string>();
                if (reader.read<bool>()) {
                    range = reader.read<float>();
                }
            }
            const auto category = reader.read<mcp::var_int>();
            const auto entity_id = reader.read<mcp::var_int>();
            const auto volume = reader.read<float>();
            const auto pitch = reader.read<float>();
            const auto seed = reader.read<std::int64_t>();

            play_entity_sound_effect_c::handler(base_handle, sound_id, sound_name, range, category, entity_id, volume, pitch, seed);
        }
    };

    template<auto Handler>
    struct play_start_configuration_c : public detail::packet_base<0x66, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(){
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            play_start_configuration_c::handler(base_handle);
        }
    };

    template<auto Handler>
    struct play_stop_sound_c : public detail::packet_base<0x68, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t flags,
                std::optional<mcp::var_int> source,
                std::optional<std::string> sound){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(flags);
            if (flags & 0x01) {
                writer.write(source.value());
            }
            if (flags & 0x02) {
                writer.write(sound.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto flags = reader.read<std::uint8_t>();
            auto sound_source = std::optional<mcp::var_int>();
            if (flags & 0x01) {
                sound_source = reader.read<mcp::var_int>();
            }
            auto sound_name = std::optional<std::string>();
            if (flags & 0x02) {
                sound_name = reader.read<std::string>();
            }

            play_stop_sound_c::handler(base_handle, flags, sound_source, sound_name);
        }
    };

    template<auto Handler>
    struct play_system_chat_message_c : public detail::packet_base<0x69, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target text,
                bool overlay){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(text, writer);
            writer.write(overlay);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto text = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            const auto overlay = reader.read<bool>();

            play_system_chat_message_c::handler(base_handle, text, overlay);
        }
    };

    template<auto Handler>
    struct play_set_tab_list_header_footer_c : public detail::packet_base<0x6A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::text_component, Converters...>::type_target header,
                detail::get_type_t<mcp::text_component, Converters...>::type_target footer){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::text_component, Converters...>::to(header, writer);
            detail::get_type_t<mcp::text_component, Converters...>::to(footer, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto header = detail::get_type_t<mcp::text_component, Converters...>::from(reader);
            const auto footer = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            play_set_tab_list_header_footer_c::handler(base_handle, header, footer);
        }
    };

    template<auto Handler>
    struct play_tag_query_response_c : public detail::packet_base<0x6B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int transaction_id,
                detail::get_type_t<mcp::nbt, Converters...>::type_target nbt){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(transaction_id);
            detail::get_type_t<mcp::nbt, Converters...>::to(nbt, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto transaction_id = reader.read<mcp::var_int>();
            const auto nbt = detail::get_type_t<mcp::nbt, Converters...>::from(reader);

            play_tag_query_response_c::handler(base_handle, transaction_id, nbt);
        }
    };

    template<auto Handler>
    struct play_pickup_item_c : public detail::packet_base<0x6C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int collected_entity_id,
                mcp::var_int collector_entity_id,
                mcp::var_int pickup_item_count){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(collected_entity_id);
            writer.write(collector_entity_id);
            writer.write(pickup_item_count);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto collected_entity_id = reader.read<mcp::var_int>();
            const auto collector_entity_id = reader.read<mcp::var_int>();
            const auto pickup_item_count = reader.read<mcp::var_int>();

            play_pickup_item_c::handler(base_handle, collected_entity_id, collector_entity_id, pickup_item_count);
        }
    };

    template<auto Handler>
    struct play_teleport_entity_c : public detail::packet_base<0x6D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                double x,
                double y,
                double z,
                std::uint8_t yaw,
                std::uint8_t pitch,
                bool on_ground){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(x);
            writer.write(y);
            writer.write(z);
            writer.write(yaw);
            writer.write(pitch);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto x = reader.read<double>();
            const auto y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto yaw = reader.read<std::uint8_t>();
            const auto pitch = reader.read<std::uint8_t>();
            const auto on_ground = reader.read<bool>();

            play_teleport_entity_c::handler(base_handle, entity_id, x, y, z, yaw, pitch, on_ground);
        }
    };

    template<auto Handler>
    struct play_set_ticking_state_c : public detail::packet_base<0x6E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float tick_rate,
                bool is_frozen){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(tick_rate);
            writer.write(is_frozen);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto tick_rate = reader.read<float>();
            const auto is_frozen = reader.read<bool>();

            play_set_ticking_state_c::handler(base_handle, tick_rate, is_frozen);
        }
    };

    template<auto Handler>
    struct play_step_tick_c : public detail::packet_base<0x6F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int steps){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(steps);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto steps = reader.read<mcp::var_int>();

            play_step_tick_c::handler(base_handle, steps);
        }
    };

    template <typename TextComponent, typename Slot>
    struct play_update_advancements_c_display {
        TextComponent title;
        TextComponent description;
        Slot icon;
        mcp::var_int frame_type;
        std::int32_t flags;
        std::optional<std::string> background_texture;
        float x_coordinate;
        float y_coordinate;
    };

    struct play_update_advancements_c_advancement_progress {
        std::string criterion_identifier;
        std::optional<std::int64_t> achieved_at;
    };
    template <typename TextComponent, typename Slot>
    struct play_update_advancements_c_advancement {
        std::optional<std::string> parent_id;
        std::optional<play_update_advancements_c_display<TextComponent, Slot>> display;
        std::vector<std::vector<std::string>> requirements;
        bool send_telemetry;
    };
    template<auto Handler>
    struct play_update_advancements_c : public detail::packet_base<0x70, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                bool reset,
                std::span<play_update_advancements_c_advancement<
                        typename detail::get_type_t<mcp::text_component, Converters...>::type_target,
                        typename detail::get_type_t<mcp::slot, Converters...>::type_target>> advancements,
                std::vector<std::string> identifiers,
                std::vector<play_update_advancements_c_advancement_progress> progress_mapping){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(reset);
            writer.write(mcp::var_int(advancements.size()));
            for (const auto& advancement : advancements) {
                writer.write(advancement.parent_id.has_value());
                if (advancement.parent_id.has_value()) {
                    writer.write(advancement.parent_id.value());
                }
                writer.write(advancement.display.has_value());
                if (advancement.display.has_value()) {
                    detail::get_type_t<mcp::text_component, Converters...>::to(advancement.display.value().title, writer);
                    detail::get_type_t<mcp::text_component, Converters...>::to(advancement.display.value().description, writer);
                    detail::get_type_t<mcp::slot, Converters...>::to(advancement.display.value().icon, writer);
                    writer.write(advancement.display.value().frame_type);
                    writer.write(advancement.display.value().flags);
                    if (advancement.display.value().background_texture.has_value()) {
                        writer.write(advancement.display.value().background_texture.value());
                    }
                    writer.write(advancement.display.value().x_coordinate);
                    writer.write(advancement.display.value().y_coordinate);
                }

                writer.write(mcp::var_int(advancement.requirements.size()));
                for (const auto& requirement : advancement.requirements) {

                    writer.write(requirement);
                }
                writer.write(advancement.send_telemetry);
            }

            writer.write(mcp::var_int(identifiers.size()));
            for (const auto& identifier : identifiers) {
                writer.write(identifier);
            }

            writer.write(mcp::var_int(progress_mapping.size()));
            for (const auto& progress : progress_mapping) {
                writer.write(progress.criterion_identifier);
                if (progress.achieved_at.has_value()) {
                    writer.write(progress.achieved_at.value());
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto reset = reader.read<bool>();
            auto mappings = std::vector<play_update_advancements_c_advancement<
                    typename detail::get_type_t<mcp::text_component, Converters...>::type_target,
                    typename detail::get_type_t<mcp::slot, Converters...>::type_target>>();
            const auto mapping_count = reader.read<mcp::var_int>();
            mappings.reserve(mapping_count.value);
            for (std::size_t i = 0; i < mapping_count.value; ++i) {
                mappings.push_back(play_update_advancements_c_advancement<
                        typename detail::get_type_t<mcp::text_component, Converters...>::type_target,
                        typename detail::get_type_t<mcp::slot, Converters...>::type_target>());
                if (reader.read<bool>()) {
                    mappings.back().parent_id = reader.read<std::string>();
                }
                if (reader.read<bool>()) {
                    detail::get_type_t<mcp::text_component, Converters...>::from(reader, mappings.back().display->title);
                    detail::get_type_t<mcp::text_component, Converters...>::from(reader, mappings.back().display->description);
                    detail::get_type_t<mcp::slot, Converters...>::from(reader, mappings.back().display->icon);
                    mappings.back().display->frame_type = reader.read<mcp::var_int>();
                    mappings.back().display->flags = reader.read<mcp::var_int>();
                    if (mappings.back().display->flags & 0x01) {
                        mappings.back().display->background_texture = reader.read<std::string>();
                    }
                    mappings.back().display->x_coordinate = reader.read<float>();
                    mappings.back().display->y_coordinate = reader.read<float>();
                }
                const auto requirements_count = reader.read<mcp::var_int>();
                mappings.back().requirements.reserve(requirements_count.value);
                for (std::size_t j = 0; j < requirements_count.value; ++j) {
                    mappings.back().requirements.push_back(reader.read<std::string>());
                }
                mappings.back().send_telemetry = reader.read<bool>();
            }

            auto identifiers = std::vector<std::string>();
            const auto identifier_count = reader.read<mcp::var_int>();
            identifiers.reserve(identifier_count.value);
            for (std::size_t i = 0; i < identifier_count.value; ++i) {
                identifiers.push_back(reader.read<std::string>());
            }

            auto progress_mapping = std::vector<play_update_advancements_c_advancement_progress>();
            const auto progress_mapping_count = reader.read<mcp::var_int>();
            progress_mapping.reserve(progress_mapping_count.value);
            for (std::size_t i = 0; i < progress_mapping_count.value; ++i) {
                progress_mapping.push_back(play_update_advancements_c_advancement_progress());
                progress_mapping.back().criterion_identifier = reader.read<std::string>();
                if (reader.read<bool>()) {
                    progress_mapping.back().achieved_at = reader.read<std::int64_t>();
                }
            }

            play_update_advancements_c::handler(base_handle, reset, mappings, identifiers, progress_mapping);
        }
    };

    template <typename UUID>
    struct play_update_attributes_c_modifier_data {
        UUID uuid;
        double amount;
        std::int8_t operation;
    };
    template <typename UUID>
    struct play_update_attributes_c_property {
        std::string key;
        double value;
        std::vector<play_update_attributes_c_modifier_data<UUID>> modifiers;
    };
    template<auto Handler>
    struct play_update_attributes_c : public detail::packet_base<0x71, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::span<play_update_attributes_c_property<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>> properties){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(mcp::var_int(properties.size()));
            for (const auto& property : properties) {
                detail::get_type_t<mcp::uuid, Converters...>::to(property.uuid, writer);
                writer.write(property.key);
                writer.write(property.value);
                writer.write(mcp::var_int(property.modifiers.size()));
                for (const auto& modifier : property.modifiers) {
                    detail::get_type_t<mcp::uuid, Converters...>::to(modifier.uuid, writer);
                    writer.write(modifier.amount);
                    writer.write(modifier.operation);
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto properties_count = reader.read<mcp::var_int>();
            std::vector<play_update_attributes_c_property<typename detail::get_type_t<mcp::uuid, Converters...>::type_target>> properties;
            properties.reserve(properties_count.value);
            for (std::size_t i = 0; i < properties_count.value; ++i) {
                play_update_attributes_c_property<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> property;
                detail::get_type_t<mcp::uuid, Converters...>::from(reader, property.uuid);
                property.key = reader.read<std::string>();
                property.value = reader.read<double>();
                const auto modifiers_count = reader.read<mcp::var_int>();
                property.modifiers.reserve(modifiers_count.value);
                for (std::size_t j = 0; j < modifiers_count.value; ++j) {
                    play_update_attributes_c_modifier_data<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> modifier;
                    detail::get_type_t<mcp::uuid, Converters...>::from(reader, modifier.uuid);
                    modifier.amount = reader.read<double>();
                    modifier.operation = reader.read<std::int8_t>();
                    property.modifiers.push_back(modifier);
                }
                properties.push_back(property);
            }

            play_update_attributes_c::handler(base_handle, entity_id, properties);
        }
    };

    template<auto Handler>
    struct play_entity_effect_c : public detail::packet_base<0x72, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                mcp::var_int effect_id,
                std::int8_t amplifier,
                mcp::var_int duration,
                std::int8_t flags,
                std::optional<typename detail::get_type_t<mcp::nbt, Converters...>::type_target> factor_data){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(effect_id);
            writer.write(amplifier);
            writer.write(duration);
            writer.write(flags);
            writer.write(factor_data.has_value());
            // Documentation doesn't say if we should write the NBT or not depending on if it exists
            // but given that literally every other packet in the protocol does it, we'll do it too.
            if (factor_data.has_value()) {
                detail::get_type_t<mcp::nbt, Converters...>::to(*factor_data, writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto effect_id = reader.read<mcp::var_int>();
            const auto amplifier = reader.read<std::int8_t>();
            const auto duration = reader.read<mcp::var_int>();
            const auto flags = reader.read<std::int8_t>();
            const auto factor_data = reader.read<bool>();
            auto factor_data_target = std::optional<typename detail::get_type_t<mcp::nbt, Converters...>::type_target>();
            if (factor_data) {
                factor_data_target = detail::get_type_t<mcp::nbt, Converters...>::from(reader);
            }

            play_entity_effect_c::handler(base_handle, entity_id, effect_id, amplifier, duration, flags, factor_data_target);
        }
    };

    template<auto Handler>
    struct play_update_recipe_c : public detail::packet_base<0x73, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            throw std::runtime_error("Not implemented");

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            throw std::runtime_error("Not implemented");

            play_update_recipe_c::handler(base_handle);
        }
    };

    struct play_update_tags_c_tag {
        std::string name;
        std::vector<mcp::var_int> ids;
    };
    struct play_update_tags_c_tag_add {
        std::string registry;
        std::vector<play_update_tags_c_tag> tags;
    };
    template<auto Handler>
    struct play_update_tags_c : public detail::packet_base<0x74, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(std::span<play_update_tags_c_tag_add> tags) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(tags.size()));
            for (const auto& tag : tags) {
                writer.write(tag.registry);
                writer.write(mcp::var_int(tag.tags.size()));
                for (const auto& t : tag.tags) {
                    writer.write(t.name);
                    writer.write(mcp::var_int(t.ids.size()));
                    for (const auto& id : t.ids) {
                        writer.write(id);
                    }
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto tags = std::vector<play_update_tags_c_tag_add>();
            const auto count = reader.read<mcp::var_int>();
            tags.reserve(count.value);
            for (std::size_t i = 0; i < count.value; ++i) {
                play_update_tags_c_tag_add tag;
                tag.registry = reader.read<std::string>();
                const auto tag_count = reader.read<mcp::var_int>();
                tag.tags.reserve(tag_count.value);
                for (std::size_t j = 0; j < tag_count.value; ++j) {
                    play_update_tags_c_tag t;
                    t.name = reader.read<std::string>();
                    const auto id_count = reader.read<mcp::var_int>();
                    t.ids.reserve(id_count.value);
                    for (std::size_t k = 0; k < id_count.value; ++k) {
                        t.ids.push_back(reader.read<mcp::var_int>());
                    }
                    tag.tags.push_back(t);
                }
                tags.push_back(tag);
            }

            play_update_tags_c::handler(base_handle, tags);
        }
    };
}

#endif //MCPPROTOCOL_PLAY_CLIENT_BOUND_HPP
