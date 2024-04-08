namespace mcp {
	template <auto Handler>
	struct login_start_s : public detail::packet_base<0x00, Handler> {
		template <typename ...Converters>
		static std::vector<std::byte> serialize(
				auto *state,
				std::string player_name,
				detail::get_type_t<mcp::uuid, Converters...>::type_target uuid) {
			auto buffer = std::vector<std::byte>();
			auto writer = mcp::writer(buffer);
	
			writer.write(player_name);
			detail::get_type_t<mcp::uuid, Converters...>::to(uuid, writer);
	
			return buffer;
		}
	
		template <typename ...Converters>
		static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
			auto reader = mcp::reader(source);
	
			const auto player_name = reader.read<std::string>();
			const auto uuid = detail::get_type_t<mcp::uuid, Converters...>::from(reader);
	
			login_start_s::handler(base_handle, player_name, uuid);
		}
	};
}
