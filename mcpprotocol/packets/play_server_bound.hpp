#ifndef MCPPROTOCOL_PLAY_SERVER_BOUND_HPP
#define MCPPROTOCOL_PLAY_SERVER_BOUND_HPP

namespace mcp {
    template<auto Handler>
    struct play_confirm_teleportation_s : public detail::packet_base<0x00, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int teleport_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(teleport_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto teleport_id = reader.read<mcp::var_int>();

            play_confirm_teleportation_s::handler(base_handle, teleport_id);
        }
    };

    template<auto Handler>
    struct play_query_block_entity_tag_s : public detail::packet_base<0x01, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int transaction_id,
                detail::get_type_t<mcp::position, Converters...>::type_target position) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(transaction_id);
            detail::get_type_t<mcp::position, Converters...>::to(position, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto transaction_id = reader.read<mcp::var_int>();
            const auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);

            play_query_block_entity_tag_s::handler(base_handle, transaction_id, position);
        }
    };

    template<auto Handler>
    struct play_change_difficulty_s : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t difficulty){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(difficulty);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto difficulty = reader.read<std::int8_t>();

            play_change_difficulty_s::handler(base_handle, difficulty);
        }
    };

    template<auto Handler>
    struct play_acknowledge_message_s : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int message_count){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(message_count);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message_count = reader.read<mcp::var_int>();

            play_acknowledge_message_s::handler(base_handle, message_count);
        }
    };

    struct play_chat_command_s_signature {
        std::string argument;
        std::vector<std::byte> signature;
    };
    template<auto Handler>
    struct play_chat_command_s : public detail::packet_base<0x04, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string command,
                std::int64_t timestamp,
                std::int64_t salt,
                std::span<play_chat_command_s_signature> signatures,
                mcp::var_int message_count,
                std::bitset<20> acknowledged){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(command);
            writer.write(timestamp);
            writer.write(salt);
            writer.write(signatures.size());
            for (const auto& signature : signatures) {
                writer.write(signature.argument);
                writer.write(signature.signature);
            }
            writer.write(message_count);
            writer.write(acknowledged);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto command = reader.read<std::string>();
            const auto timestamp = reader.read<std::int64_t>();
            const auto salt = reader.read<std::int64_t>();
            const auto signatures_count = reader.read<mcp::var_int>();
            auto signatures = std::vector<play_chat_command_s_signature>();
            signatures.reserve(signatures_count.value);
            for (std::size_t i = 0; i < signatures_count.value; ++i) {
                play_chat_command_s_signature signature;
                signature.argument = reader.read<std::string>();
                signature.signature.resize(256);
                std::memcpy(signature.signature.data(), reader.read_n(256).data(), 256);
                signatures.push_back(signature);
            }
            const auto message_count = reader.read<mcp::var_int>();
            const auto acknowledged = reader.read_bitset<20>();

            play_chat_command_s::handler(base_handle, command, timestamp, salt, signatures, message_count, acknowledged);
        }
    };

    template<auto Handler>
    struct play_chat_message_s : public detail::packet_base<0x05, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string message,
                std::int64_t timestamp,
                std::int64_t salt,
                std::optional<std::span<const std::byte>> signature,
                mcp::var_int message_count,
                std::bitset<20> acknowledged
        ){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(message);
            writer.write(timestamp);
            writer.write(salt);
            writer.write(signature.has_value());
            if (signature.has_value()) {
                writer.write(signature.value());
            }
            writer.write(message_count);
            writer.write(acknowledged);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto message = reader.read<std::string>();
            const auto timestamp = reader.read<std::int64_t>();
            const auto salt = reader.read<std::int64_t>();
            auto signature = std::optional<std::vector<std::byte>>();
            if (reader.read<bool>()) {
                signature = std::vector<std::byte>(256);
                std::memcpy(signature.value().data(), reader.read_n(256).data(), 256);
            }

            const auto message_count = reader.read<mcp::var_int>();
            const auto acknowledged = reader.read_bitset<20>();

            play_chat_message_s::handler(base_handle, message, timestamp, salt, signature, message_count, acknowledged);
        }
    };

    template<auto Handler>
    struct play_player_session_s : public detail::packet_base<0x06, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                std::int64_t expiry,
                std::span<const std::byte> public_key,
                std::span<const std::byte> signature){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(expiry);
            writer.write(mcp::var_int(public_key.size()));
            writer.write(public_key);
            writer.write(mcp::var_int(signature.size()));
            writer.write(signature);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto expiry = reader.read<std::int64_t>();
            const auto public_key_size = reader.read<mcp::var_int>();
            const auto public_key = reader.read_n(public_key_size.value);
            const auto signature_size = reader.read<mcp::var_int>();
            const auto signature = reader.read_n(signature_size.value);

            play_player_session_s::handler(base_handle, uuid, expiry, public_key, signature);
        }
    };

    template<auto Handler>
    struct play_chunk_batch_received_s : public detail::packet_base<0x07, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float chunks_per_tick){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(chunks_per_tick);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto chunks_per_tick = reader.read<float>();

            play_chunk_batch_received_s::handler(base_handle, chunks_per_tick);
        }
    };

    template<auto Handler>
    struct play_client_status_s : public detail::packet_base<0x08, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(mcp::var_int action) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(action);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto action = reader.read<mcp::var_int>();

            play_client_status_s::handler(base_handle, action);
        }
    };

    template<auto Handler>
    struct play_client_information_s : public detail::packet_base<0x09, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string locale,
                std::int8_t view_distance,
                mcp::var_int chat_mode,
                bool chat_colors,
                std::uint8_t displayed_skin_parts,
                mcp::var_int main_hand,
                bool enable_text_filtering,
                bool allow_server_listing){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(locale);
            writer.write(view_distance);
            writer.write(chat_mode);
            writer.write(chat_colors);
            writer.write(displayed_skin_parts);
            writer.write(main_hand);
            writer.write(enable_text_filtering);
            writer.write(allow_server_listing);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto locale = reader.read<std::string>();
            const auto view_distance = reader.read<std::int8_t>();
            const auto chat_mode = reader.read<mcp::var_int>();
            const auto chat_colors = reader.read<bool>();
            const auto displayed_skin_parts = reader.read<std::uint8_t>();
            const auto main_hand = reader.read<mcp::var_int>();
            const auto enable_text_filtering = reader.read<bool>();
            const auto allow_server_listing = reader.read<bool>();

            play_client_information_s::handler(base_handle, locale, view_distance, chat_mode, chat_colors, displayed_skin_parts, main_hand, enable_text_filtering, allow_server_listing);
        }
    };

    template<auto Handler>
    struct play_command_suggestions_request_s : public detail::packet_base<0x0A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int transaction_id,
                std::string command){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(transaction_id);
            writer.write(command);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto transaction_id = reader.read<mcp::var_int>();
            const auto command = reader.read<std::string>();

            play_command_suggestions_request_s::handler(base_handle, transaction_id, command);
        }
    };

    template<auto Handler>
    struct play_acknowledge_configuration_s : public detail::packet_base<0x0B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(){
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            play_acknowledge_configuration_s::handler(base_handle);
        }
    };

    template<auto Handler>
    struct play_click_container_button_s : public detail::packet_base<0x0C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t window_id,
                std::int8_t button_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(button_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::int8_t>();
            const auto button_id = reader.read<std::int8_t>();

            play_click_container_button_s::handler(base_handle, window_id, button_id);
        }
    };

    template <typename SlotT>
    struct play_click_container_s_changed_slot {
        std::int16_t number;
        SlotT slot;
    };
    template<auto Handler>
    struct play_click_container_s : public detail::packet_base<0x0D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t window_id,
                mcp::var_int state_id,
                std::int16_t slot,
                std::int8_t button,
                mcp::var_int mode,
                std::span<play_click_container_s_changed_slot<typename detail::get_type_t<mcp::slot, Converters...>::type_target>> slots,
                detail::get_type_t<mcp::slot, Converters...>::type_target carried_item){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(state_id);
            writer.write(slot);
            writer.write(button);
            writer.write(mode);
            writer.write(mcp::var_int(slots.size()));
            for (const auto &s : slots) {
                writer.write(s.number);
                detail::get_type_t<mcp::slot, Converters...>::to(s.slot, writer);
            }
            detail::get_type_t<mcp::slot, Converters...>::to(carried_item, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();
            const auto state_id = reader.read<mcp::var_int>();
            const auto slot = reader.read<std::int16_t>();
            const auto button = reader.read<std::int8_t>();
            const auto mode = reader.read<mcp::var_int>();
            const auto slots_count = reader.read<mcp::var_int>();
            std::vector<play_click_container_s_changed_slot<typename detail::get_type_t<mcp::slot, Converters...>::type_target>> slots;
            slots.reserve(slots_count.value);
            for (std::size_t i = 0; i < slots_count.value; ++i) {
                play_click_container_s_changed_slot<typename detail::get_type_t<mcp::slot, Converters...>::type_target> slot;
                slot.number = reader.read<std::int16_t>();
                detail::get_type_t<mcp::slot, Converters...>::from(reader, slot.slot);
                slots.push_back(slot);
            }
            const auto carried_item = detail::get_type_t<mcp::slot, Converters...>::from(reader);

            play_click_container_s::handler(base_handle, window_id, state_id, slot, button, mode, slots, carried_item);
        }
    };

    template<auto Handler>
    struct play_close_container_s : public detail::packet_base<0x0E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::uint8_t window_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::uint8_t>();

            play_close_container_s::handler(base_handle, window_id);
        }
    };

    template<auto Handler>
    struct play_change_container_slot_state_s : public detail::packet_base<0x0F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int slot_id,
                mcp::var_int window_id,
                bool state){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot_id);
            writer.write(window_id);
            writer.write(state);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot_id = reader.read<mcp::var_int>();
            const auto window_id = reader.read<mcp::var_int>();
            const auto state = reader.read<bool>();

            play_change_container_slot_state_s::handler(base_handle, slot_id, window_id, state);
        }
    };

    template<auto Handler>
    struct play_server_bound_plugin_message_s : public detail::packet_base<0x10, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string channel,
                std::span<std::byte> data){
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

            play_server_bound_plugin_message_s::handler(base_handle, channel, data);
        }
    };

    template<auto Handler>
    struct play_edit_book_s : public detail::packet_base<0x11, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int slot,
                mcp::var_int count,
                std::span<std::string> entries,
                std::optional<std::string> title){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot);
            writer.write(count);
            writer.write(mcp::var_int(entries.size()));
            for (const auto &entry : entries) {
                writer.write(entry);
            }
            writer.write(title.has_value());
            if (title.has_value()) {
                writer.write(title.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot = reader.read<mcp::var_int>();
            const auto count = reader.read<mcp::var_int>();
            const auto entries_count = reader.read<mcp::var_int>();
            std::vector<std::string> entries;
            entries.reserve(entries_count.value);
            for (std::size_t i = 0; i < entries_count.value; ++i) {
                entries.push_back(reader.read<std::string>());
            }
            auto title = std::optional<std::string>();
            if (reader.read<bool>()) {
                title = reader.read<std::string>();
            }

            play_edit_book_s::handler(base_handle, slot, count, entries, title);
        }
    };

    template<auto Handler>
    struct play_query_entity_tag_s : public detail::packet_base<0x12, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int transaction_id,
                mcp::var_int entity_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(transaction_id);
            writer.write(entity_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto transaction_id = reader.read<mcp::var_int>();
            const auto entity_id = reader.read<mcp::var_int>();

            play_query_entity_tag_s::handler(base_handle, transaction_id, entity_id);
        }
    };

    template<auto Handler>
    struct play_interact_s : public detail::packet_base<0x13, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                mcp::var_int type,
                std::optional<float> target_x,
                std::optional<float> target_y,
                std::optional<float> target_z,
                std::optional<mcp::var_int> hand,
                bool sneaking){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(type);
            if (type.value == 2) {
                writer.write(target_x.value());
                writer.write(target_y.value());
                writer.write(target_z.value());
            }
            if (type.value == 1 || type.value == 2) {
                writer.write(hand.value());
            }

            writer.write(sneaking);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto type = reader.read<mcp::var_int>();
            auto target_x = std::optional<float>();
            auto target_y = std::optional<float>();
            auto target_z = std::optional<float>();
            auto hand = std::optional<mcp::var_int>();
            if (type.value == 2) {
                target_x = reader.read<float>();
                target_y = reader.read<float>();
                target_z = reader.read<float>();
            }
            if (type.value == 1 || type.value == 2) {
                hand = reader.read<mcp::var_int>();
            }
            const auto sneaking = reader.read<bool>();

            play_interact_s::handler(base_handle, entity_id, type, target_x, target_y, target_z, hand, sneaking);
        }
    };

    template<auto Handler>
    struct play_jigsaw_generate_s : public detail::packet_base<0x14, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                mcp::var_int levels,
                bool keep_jigsaws){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(levels);
            writer.write(keep_jigsaws);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto levels = reader.read<mcp::var_int>();
            const auto keep_jigsaws = reader.read<bool>();

            play_jigsaw_generate_s::handler(base_handle, location, levels, keep_jigsaws);
        }
    };

    template<auto Handler>
    struct play_server_bound_keep_alive_s : public detail::packet_base<0x15, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t keep_alive_id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(keep_alive_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto keep_alive_id = reader.read<std::int64_t>();

            play_server_bound_keep_alive_s::handler(base_handle, keep_alive_id);
        }
    };

    template<auto Handler>
    struct play_lock_difficulty_s : public detail::packet_base<0x16, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                bool locked){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(locked);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto locked = reader.read<bool>();

            play_lock_difficulty_s::handler(base_handle, locked);
        }
    };

    template<auto Handler>
    struct play_set_player_position_s : public detail::packet_base<0x17, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double feet_y,
                double z,
                bool on_ground){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(feet_y);
            writer.write(z);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto feet_y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto on_ground = reader.read<bool>();

            play_set_player_position_s::handler(base_handle, x, feet_y, z, on_ground);
        }
    };

    template<auto Handler>
    struct play_set_player_position_rotation_s : public detail::packet_base<0x18, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double feet_y,
                double z,
                float yaw,
                float pitch,
                bool on_ground){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(x);
            writer.write(feet_y);
            writer.write(z);
            writer.write(yaw);
            writer.write(pitch);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto x = reader.read<double>();
            const auto feet_y = reader.read<double>();
            const auto z = reader.read<double>();
            const auto yaw = reader.read<float>();
            const auto pitch = reader.read<float>();
            const auto on_ground = reader.read<bool>();

            play_set_player_position_rotation_s::handler(base_handle, x, feet_y, z, yaw, pitch, on_ground);
        }
    };

    template<auto Handler>
    struct play_set_player_rotation_s : public detail::packet_base<0x19, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float yaw,
                float pitch,
                bool on_ground){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(yaw);
            writer.write(pitch);
            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto yaw = reader.read<float>();
            const auto pitch = reader.read<float>();
            const auto on_ground = reader.read<bool>();

            play_set_player_rotation_s::handler(base_handle, yaw, pitch, on_ground);
        }
    };

    template<auto Handler>
    struct play_set_player_on_ground_s : public detail::packet_base<0x1A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                bool on_ground){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(on_ground);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto on_ground = reader.read<bool>();

            play_set_player_on_ground_s::handler(base_handle, on_ground);
        }
    };

    template<auto Handler>
    struct play_move_vehicle_s : public detail::packet_base<0x1B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                double x,
                double y,
                double z,
                float yaw,
                float pitch){
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
            const auto yaw = reader.read<float>();
            const auto pitch = reader.read<float>();

            play_move_vehicle_s::handler(base_handle, x, y, z, yaw, pitch);
        }
    };

    template<auto Handler>
    struct play_paddle_boat_s : public detail::packet_base<0x1C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                bool left_paddle,
                bool right_paddle){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(left_paddle);
            writer.write(right_paddle);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto left_paddle = reader.read<bool>();
            const auto right_paddle = reader.read<bool>();

            play_paddle_boat_s::handler(base_handle, left_paddle, right_paddle);
        }
    };

    template<auto Handler>
    struct play_pick_item_s : public detail::packet_base<0x1D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int slot_to_use){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot_to_use);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot_to_use = reader.read<mcp::var_int>();

            play_pick_item_s::handler(base_handle, slot_to_use);
        }
    };

    template<auto Handler>
    struct play_ping_request_s : public detail::packet_base<0x1E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t payload){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(payload);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto payload = reader.read<std::int64_t>();

            play_ping_request_s::handler(base_handle, payload);
        }
    };

    template<auto Handler>
    struct play_place_recipe_s : public detail::packet_base<0x1F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t window_id,
                std::string recipe,
                bool make_all){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(window_id);
            writer.write(recipe);
            writer.write(make_all);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto window_id = reader.read<std::int8_t>();
            const auto recipe = reader.read<std::string>();
            const auto make_all = reader.read<bool>();

            play_place_recipe_s::handler(base_handle, window_id, recipe, make_all);
        }
    };

    template<auto Handler>
    struct play_player_abilities_s : public detail::packet_base<0x20, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int8_t flags){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto flags = reader.read<std::int8_t>();

            play_player_abilities_s::handler(base_handle, flags);
        }
    };

    template<auto Handler>
    struct play_player_action_s : public detail::packet_base<0x21, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int status,
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                std::int8_t face,
                mcp::var_int sequence){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(status);
            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(face);
            writer.write(sequence);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto status = reader.read<mcp::var_int>();
            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto face = reader.read<std::int8_t>();
            const auto sequence = reader.read<mcp::var_int>();

            play_player_action_s::handler(base_handle, status, location, face, sequence);
        }
    };

    template<auto Handler>
    struct play_player_command_s : public detail::packet_base<0x22, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                mcp::var_int action_id,
                mcp::var_int jump_boost){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(action_id);
            writer.write(jump_boost);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto action_id = reader.read<mcp::var_int>();
            const auto jump_boost = reader.read<mcp::var_int>();

            play_player_command_s::handler(base_handle, entity_id, action_id, jump_boost);
        }
    };

    template<auto Handler>
    struct play_player_input_s : public detail::packet_base<0x23, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                float sideways,
                float forward,
                std::uint8_t flags){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(sideways);
            writer.write(forward);
            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto sideways = reader.read<float>();
            const auto forward = reader.read<float>();
            const auto flags = reader.read<std::uint8_t>();

            play_player_input_s::handler(base_handle, sideways, forward, flags);
        }
    };

    template<auto Handler>
    struct play_pong_s : public detail::packet_base<0x24, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int32_t id){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto id = reader.read<std::int32_t>();

            play_pong_s::handler(base_handle, id);
        }
    };

    template<auto Handler>
    struct play_change_recipe_book_settings_s : public detail::packet_base<0x25, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int book_id,
                bool open,
                bool filter){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(book_id);
            writer.write(open);
            writer.write(filter);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto book_id = reader.read<mcp::var_int>();
            const auto open = reader.read<bool>();
            const auto filter = reader.read<bool>();

            play_change_recipe_book_settings_s::handler(base_handle, book_id, open, filter);
        }
    };

    template<auto Handler>
    struct play_set_seen_recipe_s : public detail::packet_base<0x26, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string recipe){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(recipe);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto recipe = reader.read<std::string>();

            play_set_seen_recipe_s::handler(base_handle, recipe);
        }
    };

    template<auto Handler>
    struct play_rename_item_s : public detail::packet_base<0x27, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::string name){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(name);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto name = reader.read<std::string>();

            play_rename_item_s::handler(base_handle, name);
        }
    };

    template<auto Handler>
    struct play_resource_pack_response_s : public detail::packet_base<0x28, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target id,
                mcp::var_int result){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(id, writer);
            writer.write(result);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto id = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto result = reader.read<mcp::var_int>();

            play_resource_pack_response_s::handler(base_handle, id, result);
        }
    };

    template<auto Handler>
    struct play_seen_advancements_s : public detail::packet_base<0x29, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int action,
                std::optional<std::string> tab){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(action);
            if (action.value == 0) {
                writer.write(tab.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto action = reader.read<mcp::var_int>();
            auto tab = std::optional<std::string>();
            if (action.value == 0) {
                tab = reader.read<std::string>();
            }

            play_seen_advancements_s::handler(base_handle, action, tab);
        }
    };

    template<auto Handler>
    struct play_select_trade_s : public detail::packet_base<0x2A, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int selected_slot){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(selected_slot);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto selected_slot = reader.read<mcp::var_int>();

            play_select_trade_s::handler(base_handle, selected_slot);
        }
    };

    template<auto Handler>
    struct play_set_beacon_effect_s : public detail::packet_base<0x2B, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::optional<mcp::var_int> primary_effect,
                std::optional<mcp::var_int> secondary_effect){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(primary_effect.has_value());
            if (primary_effect.has_value()) {
                writer.write(primary_effect.value());
            }
            writer.write(secondary_effect.has_value());
            if (secondary_effect.has_value()) {
                writer.write(secondary_effect.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto primary_effect = std::optional<mcp::var_int>();
            if (reader.read<bool>()) {
                primary_effect = reader.read<mcp::var_int>();
            }
            auto secondary_effect = std::optional<mcp::var_int>();
            if (reader.read<bool>()) {
                secondary_effect = reader.read<mcp::var_int>();
            }

            play_set_beacon_effect_s::handler(base_handle, primary_effect, secondary_effect);

        }
    };

    template<auto Handler>
    struct play_set_held_item_s : public detail::packet_base<0x2C, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int16_t slot){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot = reader.read<std::int16_t>();

            play_set_held_item_s::handler(base_handle, slot);
        }
    };

    template<auto Handler>
    struct play_program_command_block_s : public detail::packet_base<0x2D, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                std::string command,
                mcp::var_int mode,
                std::int8_t flags){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(command);
            writer.write(mode);
            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto command = reader.read<std::string>();
            const auto mode = reader.read<mcp::var_int>();
            const auto flags = reader.read<std::int8_t>();

            play_program_command_block_s::handler(base_handle, location, command, mode, flags);
        }
    };

    template<auto Handler>
    struct play_program_command_block_minecraft_s : public detail::packet_base<0x2E, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int entity_id,
                std::string command,
                bool track_output){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(entity_id);
            writer.write(command);
            writer.write(track_output);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entity_id = reader.read<mcp::var_int>();
            const auto command = reader.read<std::string>();
            const auto track_output = reader.read<bool>();

            play_program_command_block_minecraft_s::handler(base_handle, entity_id, command, track_output);
        }
    };

    template<auto Handler>
    struct play_set_creative_mode_slot_s : public detail::packet_base<0x2F, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int16_t slot,
                detail::get_type_t<mcp::slot, Converters...>::type_target clicked_item){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(slot);
            detail::get_type_t<mcp::slot, Converters...>::to(clicked_item, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto slot = reader.read<std::int16_t>();
            const auto clicked_item = detail::get_type_t<mcp::slot, Converters...>::from(reader);

            play_set_creative_mode_slot_s::handler(base_handle, slot, clicked_item);
        }
    };

    template<auto Handler>
    struct play_program_jigsaw_block_s : public detail::packet_base<0x30, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                std::string name,
                std::string target,
                std::string pool,
                std::string final_state,
                std::string joint_type,
                mcp::var_int selection_priority,
                mcp::var_int placement_priority){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(name);
            writer.write(target);
            writer.write(pool);
            writer.write(final_state);
            writer.write(joint_type);
            writer.write(selection_priority);
            writer.write(placement_priority);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto name = reader.read<std::string>();
            const auto target = reader.read<std::string>();
            const auto pool = reader.read<std::string>();
            const auto final_state = reader.read<std::string>();
            const auto joint_type = reader.read<std::string>();
            const auto selection_priority = reader.read<mcp::var_int>();
            const auto placement_priority = reader.read<mcp::var_int>();

            play_program_jigsaw_block_s::handler(base_handle, location, name, target, pool, final_state, joint_type, selection_priority, placement_priority);
        }
    };

    template<auto Handler>
    struct play_program_structure_block_s : public detail::packet_base<0x31, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                mcp::var_int action,
                mcp::var_int mode,
                std::string name,
                std::int8_t offset_x,
                std::int8_t offset_y,
                std::int8_t offset_z,
                std::int8_t size_x,
                std::int8_t size_y,
                std::int8_t size_z,
                mcp::var_int mirror,
                mcp::var_int rotation,
                std::string metadata,
                float integrity,
                mcp::var_long seed,
                std::int8_t flags){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(action);
            writer.write(mode);
            writer.write(name);
            writer.write(offset_x);
            writer.write(offset_y);
            writer.write(offset_z);
            writer.write(size_x);
            writer.write(size_y);
            writer.write(size_z);
            writer.write(mirror);
            writer.write(rotation);
            writer.write(metadata);
            writer.write(integrity);
            writer.write(seed);
            writer.write(flags);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto action = reader.read<mcp::var_int>();
            const auto mode = reader.read<mcp::var_int>();
            const auto name = reader.read<std::string>();
            const auto offset_x = reader.read<std::int8_t>();
            const auto offset_y = reader.read<std::int8_t>();
            const auto offset_z = reader.read<std::int8_t>();
            const auto size_x = reader.read<std::int8_t>();
            const auto size_y = reader.read<std::int8_t>();
            const auto size_z = reader.read<std::int8_t>();
            const auto mirror = reader.read<mcp::var_int>();
            const auto rotation = reader.read<mcp::var_int>();
            const auto metadata = reader.read<std::string>();
            const auto integrity = reader.read<float>();
            const auto seed = reader.read<mcp::var_long>();
            const auto flags = reader.read<std::int8_t>();

            play_program_structure_block_s::handler(base_handle, location, action, mode, name, offset_x, offset_y, offset_z, size_x, size_y, size_z, mirror, rotation, metadata, integrity, seed, flags);
        }
    };

    template<auto Handler>
    struct play_update_sign_s : public detail::packet_base<0x32, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::position, Converters...>::type_target location,
                bool is_front_text,
                std::string line_1,
                std::string line_2,
                std::string line_3,
                std::string line_4){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::position, Converters...>::to(location, writer);
            writer.write(is_front_text);
            writer.write(line_1);
            writer.write(line_2);
            writer.write(line_3);
            writer.write(line_4);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto location = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto is_front_text = reader.read<bool>();
            const auto line_1 = reader.read<std::string>();
            const auto line_2 = reader.read<std::string>();
            const auto line_3 = reader.read<std::string>();
            const auto line_4 = reader.read<std::string>();

            play_update_sign_s::handler(base_handle, location, is_front_text, line_1, line_2, line_3, line_4);
        }
    };

    template<auto Handler>
    struct play_swing_arm_s : public detail::packet_base<0x33, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int hand){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(hand);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto hand = reader.read<mcp::var_int>();

            play_swing_arm_s::handler(base_handle, hand);
        }
    };

    template<auto Handler>
    struct play_teleport_to_entity_s : public detail::packet_base<0x34, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);

            play_teleport_to_entity_s::handler(base_handle, uuid);
        }
    };

    template<auto Handler>
    struct play_use_item_on_s : public detail::packet_base<0x35, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int hand,
                detail::get_type_t<mcp::position, Converters...>::type_target position,
                mcp::var_int face,
                float cursor_x,
                float cursor_y,
                float cursor_z,
                bool inside_block,
                mcp::var_int sequence){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(hand);
            detail::get_type_t<mcp::position, Converters...>::to(position, writer);
            writer.write(face);
            writer.write(cursor_x);
            writer.write(cursor_y);
            writer.write(cursor_z);
            writer.write(inside_block);
            writer.write(sequence);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto hand = reader.read<mcp::var_int>();
            const auto position = detail::get_type_t<mcp::position, Converters...>::from(reader);
            const auto face = reader.read<mcp::var_int>();
            const auto cursor_x = reader.read<float>();
            const auto cursor_y = reader.read<float>();
            const auto cursor_z = reader.read<float>();
            const auto inside_block = reader.read<bool>();
            const auto sequence = reader.read<mcp::var_int>();

            play_use_item_on_s::handler(base_handle, hand, position, face, cursor_x, cursor_y, cursor_z, inside_block, sequence);
        }
    };

    template<auto Handler>
    struct play_use_item_s : public detail::packet_base<0x36, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                mcp::var_int hand,
                mcp::var_int sequence){
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(hand);
            writer.write(sequence);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto hand = reader.read<mcp::var_int>();
            const auto sequence = reader.read<mcp::var_int>();

            play_use_item_s::handler(base_handle, hand, sequence);
        }
    };
}

#endif //MCPPROTOCOL_PLAY_SERVER_BOUND_HPP
