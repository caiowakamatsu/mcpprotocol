#include <iostream>

#include <sockpp/tcp_connector.h>

#include <mcpprotocol/mcpprotocol.hpp>

#include <string>
#include <thread>

[[nodiscard]] sockpp::tcp_connector connect_to_localhost() {
    auto connection = sockpp::tcp_connector();
    constexpr auto port = 25565;

    if (!connection.connect(sockpp::inet_address("localhost", port))) {
        std::cerr << "Failed to connect to localhost:" << port << std::endl;
    }

    connection.set_non_blocking(true);

    return connection;
}

void handle_status_response(std::string json_response) {
    std::cout << "Status response: " << json_response << std::endl;
}

int main() {
    std::cout << "Connecting to localhost" << std::endl;
    auto connection = connect_to_localhost();

    using protocol = mcp::protocol<mcp::version::v765>;
    auto network_state = mcp::basic_network_state<void>();

    auto bytes = protocol::serialize<mcp::handshake_s>(network_state,
                                                             765,
                                                             "localhost",
                                                             25565,
                                                             1);
    connection.write(bytes.data(), bytes.size());

    bytes = protocol::serialize<mcp::status_status_request_s>(network_state);
    connection.write(bytes.data(), bytes.size());

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto deserializer = protocol::deserializer<
            mcp::status_status_response_c<handle_status_response>>();

    auto read_buffer = std::vector<std::byte>(1024);
    connection.read(read_buffer.data(), read_buffer.size());
    deserializer.decode(network_state, read_buffer);

    connection.close();
}