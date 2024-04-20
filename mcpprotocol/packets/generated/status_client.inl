namespace mcp {
	template <auto Handler>
	struct status_response_c : public detail::packet_base<0x00, Handler> {
		template <typename ...Converters>
		static std::vector<std::byte> serialize(
				auto *state,
				std::string json_response) {
			auto buffer = std::vector<std::byte>();
			auto writer = mcp::writer(buffer);
	
			writer.write(json_response);
	
			return buffer;
		}
	
		template <typename ...Converters>
		static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
			auto reader = mcp::reader(source);
	
			const auto json_response = reader.read<std::string>();
	
			status_response_c::handler(base_handle, json_response);
		}
	};
	template <auto Handler>
	struct ping_response_c : public detail::packet_base<0x01, Handler> {
		template <typename ...Converters>
		static std::vector<std::byte> serialize(
				auto *state,
				std::int64_t payload) {
			auto buffer = std::vector<std::byte>();
			auto writer = mcp::writer(buffer);
	
			writer.write(payload);
	
			return buffer;
		}
	
		template <typename ...Converters>
		static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
			auto reader = mcp::reader(source);
	
			const auto payload = reader.read<std::int64_t>();
	
			ping_response_c::handler(base_handle, payload);
		}
	};
}
