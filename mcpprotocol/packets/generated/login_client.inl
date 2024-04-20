namespace mcp {
	template <auto Handler>
	struct disconnect_c : public detail::packet_base<0x00, Handler> {
		template <typename ...Converters>
		static std::vector<std::byte> serialize(
				auto *state,
				std::string reason) {
			auto buffer = std::vector<std::byte>();
			auto writer = mcp::writer(buffer);
	
			writer.write(reason);
	
			return buffer;
		}
	
		template <typename ...Converters>
		static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
			auto reader = mcp::reader(source);
	
			const auto reason = reader.read<std::string>();
	
			disconnect_c::handler(base_handle, reason);
		}
	};
}
