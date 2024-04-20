namespace mcp {
	template <auto Handler>
	struct status_request_s : public detail::packet_base<0x00, Handler> {
		template <typename ...Converters>
		static std::vector<std::byte> serialize(
				auto *state) {
			auto buffer = std::vector<std::byte>();
			auto writer = mcp::writer(buffer);
	
	
			return buffer;
		}
	
		template <typename ...Converters>
		static void handle(auto base_handle, auto *state, std::span<const std::byte> source) {
			auto reader = mcp::reader(source);
	
	
			status_request_s::handler(base_handle);
		}
	};
	template <auto Handler>
	struct ping_request_s : public detail::packet_base<0x01, Handler> {
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
	
			ping_request_s::handler(base_handle, payload);
		}
	};
}
