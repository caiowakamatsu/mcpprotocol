#include <catch2/catch_all.hpp>

#include <mcpprotocol/packet_base.hpp>
#include <mcpprotocol/deserializer.hpp>

template <auto Handler>
struct fake_packet : mcp::packet_base<Handler> {
    constexpr static std::uint32_t id = 0;

    template <typename ...Converters>
    static bool handle(auto base_handle, auto *state, std::string_view source) {
        auto first_whitespace = 0;
        while (*(source.begin() + ++first_whitespace) != ' ');
        auto str = std::string(source.begin(), source.begin() + first_whitespace);
        auto number = std::stoi(std::string(source.begin() + first_whitespace, source.end()));
        fake_packet::handler(base_handle, str, number);
        return true;
    };
};

TEST_CASE("Initialization", "[deserializer]") {
//    using deserializer = mcp::basic_deserializer;

//    const auto play_deserializer = mcp::deserializer::with_packets<
//        mcp::player_move<move_handler>,
//    >();
//
//    auto state = mcp::basic_network_state<std::uint64_t>();
//    state.user_data = 3049205;
//
//    std::cout << state.counter << ", " << state.user_data << std::endl;
//    play_deserializer.ingest(bytes_from_network, state);
}
