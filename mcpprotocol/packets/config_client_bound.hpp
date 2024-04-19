#ifndef MCPPROTOCOL_CONFIG_CLIENT_BOUND_HPP
#define MCPPROTOCOL_CONFIG_CLIENT_BOUND_HPP

#include <mcpprotocol/types.hpp>
#include <mcpprotocol/packet_base.hpp>
#include <mcpprotocol/reader.hpp>
#include <mcpprotocol/writer.hpp>

namespace mcp {
    template<auto Handler>
    struct config_plugin_message_c : public detail::packet_base<0x00, Handler> {
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

            config_plugin_message_c::handler(base_handle, channel, data);
        }
    };

    template<auto Handler>
    struct config_disconnect_c : public detail::packet_base<0x01, Handler> {
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

            auto reason = detail::get_type_t<mcp::text_component, Converters...>::from(reader);

            config_disconnect_c::handler(base_handle, reason);
        }
    };

    template<auto Handler>
    struct config_finish_configuration_c : public detail::packet_base<0x02, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize() {
            return {};
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            config_finish_configuration_c::handler(base_handle);
        }
    };

    template<auto Handler>
    struct config_keep_alive_c : public detail::packet_base<0x03, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::int64_t keep_alive_id) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(keep_alive_id);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto keep_alive_id = reader.read<std::int64_t>();

            config_keep_alive_c::handler(base_handle, keep_alive_id);
        }
    };

    template<auto Handler>
    struct config_ping_c : public detail::packet_base<0x04, Handler> {
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

            config_ping_c::handler(base_handle, id);
        }
    };

    template<auto Handler>
    struct config_registry_data_c : public detail::packet_base<0x05, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::nbt, Converters...>::type_target nbt) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::nbt, Converters...>::to(nbt, writer);

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto nbt = detail::get_type_t<mcp::nbt, Converters...>::from(reader);

            config_registry_data_c::handler(base_handle, nbt);
        }
    };

    template<auto Handler>
    struct config_remove_resource_pack_c : public detail::packet_base<0x06, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::optional<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> uuid) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(uuid.has_value());
            if (uuid.has_value()) {
                detail::get_type_t<mcp::uuid, Converters...>::to(uuid.value(), writer);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto has_uuid = reader.read<bool>();

            std::optional<typename detail::get_type_t<mcp::uuid, Converters...>::type_target> uuid;
            if (has_uuid) {
                uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            }

            config_remove_resource_pack_c::handler(base_handle, uuid);
        }
    };

    template<auto Handler>
    struct config_add_resource_pack_c : public detail::packet_base<0x07, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                detail::get_type_t<mcp::uuid, Converters...>::type_target uuid,
                std::string url,
                std::string hash,
                bool forced,
                std::optional<std::string> prompt_message) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
            writer.write(url);
            writer.write(mcp::var_int(hash.size()));
            writer.write(hash);
            writer.write(forced);
            writer.write(prompt_message.has_value());
            if (prompt_message.has_value()) {
                writer.write(prompt_message.value());
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
            const auto url = reader.read<std::string>();
            const auto hash_length = reader.read<mcp::var_int>();
            const auto hash = reader.read_n(hash_length.value);
            const auto forced = reader.read<bool>();
            auto prompt_message = std::optional<std::string>();
            if (reader.read<bool>()) {
                prompt_message = reader.read<std::string>();
            }

            config_add_resource_pack_c::handler(base_handle, uuid, url, hash, forced, prompt_message);
        }
    };

    template<auto Handler>
    struct config_feature_flags_c : public detail::packet_base<0x08, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<std::string> feature_flags) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(feature_flags.size()));
            for (const auto &feature_flag : feature_flags) {
                writer.write(feature_flag);
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto feature_flags_count = reader.read<mcp::var_int>();
            std::vector<std::string> feature_flags;
            feature_flags.reserve(feature_flags_count.value);

            for (std::size_t i = 0; i < feature_flags_count.value; ++i) {
                feature_flags.push_back(reader.read<std::string>());
            }

            config_feature_flags_c::handler(base_handle, feature_flags);
        }
    };

    struct config_tag {
        std::string identifier;
        std::vector<std::int32_t> values;
    };

    struct config_tag_entry {
        std::string identifier;
        std::vector<config_tag> tags;
    };
    template<auto Handler>
    struct config_update_tags_c : public detail::packet_base<0x09, Handler> {
        template <typename ...Converters>
        static std::vector<std::byte> serialize(
                std::span<config_tag_entry> entries) {
            auto buffer = std::vector<std::byte>();
            auto writer = mcp::writer(buffer);

            writer.write(mcp::var_int(entries.size()));
            for (const auto &entry : entries) {
                writer.write(entry.identifier);
                writer.write(mcp::var_int(entry.tags.size()));
                for (const auto &tag : entry.tags) {
                    writer.write(tag.identifier);
                    writer.write(mcp::var_int(tag.values.size()));
                    for (const auto &value : tag.values) {
                        writer.write(value);
                    }
                }
            }

            return buffer;
        }

        template <typename ...Converters>
        static void handle(auto base_handle, std::span<const std::byte> source) {
            auto reader = mcp::reader(source);

            const auto entries_count = reader.read<mcp::var_int>();
            std::vector<config_tag_entry> entries;
            entries.reserve(entries_count.value);

            for (std::size_t i = 0; i < entries_count.value; ++i) {
                config_tag_entry entry;
                entry.identifier = reader.read<std::string>();
                const auto tags_count = reader.read<mcp::var_int>();
                entry.tags.reserve(tags_count.value);
                for (std::size_t j = 0; j < tags_count.value; ++j) {
                    config_tag tag;
                    tag.identifier = reader.read<std::string>();
                    const auto values_count = reader.read<mcp::var_int>();
                    tag.values.reserve(values_count.value);
                    for (std::size_t k = 0; k < values_count.value; ++k) {
                        tag.values.push_back(reader.read<std::int32_t>());
                    }
                    entry.tags.push_back(tag);
                }
                entries.push_back(entry);
            }

            config_update_tags_c::handler(base_handle, entries);
        }
    };
}

#endif //MCPPROTOCOL_CONFIG_CLIENT_BOUND_HPP
