#include <iostream>

#include <sockpp/tcp_connector.h>

#include <mcpprotocol/mcpprotocol.hpp>

#include <string>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

[[nodiscard]] sockpp::tcp_connector connect_to_localhost() {
    auto connection = sockpp::tcp_connector();
    constexpr auto port = 25565;

    if (!connection.connect(sockpp::inet_address("localhost", port))) {
        std::cerr << "Failed to connect to localhost:" << port << std::endl;
    }

    connection.set_non_blocking(true);

    return connection;
}

struct status_response_handler {
    bool wants_ping = false;
    void handle(std::string json_response) {
        std::cout << "Status response: " << json_response << std::endl;
        wants_ping = true;
    }
};
void handle_status_response(std::string json_response) {
    std::cout << "Status response: " << json_response << std::endl;
}
struct ping_response_handler {
    bool ready = false;
    void handle(std::uint64_t payload) {
        std::cout << "Ping response: " << payload << std::endl;
        ready = true;
    }
};

int main() {
    sockpp::initialize();

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

    auto status_handler = status_response_handler();
    auto handler = ping_response_handler();

    auto deserializer = protocol::deserializer<
            mcp::status_status_response_c<&status_response_handler::handle>,
            mcp::status_ping_response_c<&ping_response_handler::handle>>(&status_handler, &handler);

    auto read_buffer = std::vector<std::byte>(8);

    bytes = protocol::serialize<mcp::status_status_request_s>(network_state);
    connection.write(bytes.data(), bytes.size());

    while (!handler.ready) {
        if (status_handler.wants_ping) {
            bytes = protocol::serialize<mcp::status_ping_request_s>(network_state, 123456789);
            connection.write(bytes.data(), bytes.size());
            status_handler.wants_ping = false;
        }

        const auto bytes_read = connection.read(read_buffer.data(), read_buffer.size());

        if (bytes_read > 0) {
            deserializer.decode(network_state, {read_buffer.data(), static_cast<std::size_t>(bytes_read)});
        } else if (!(
#ifdef _WIN32
                WSAGetLastError() == WSAEWOULDBLOCK
#else
                errno == EAGAIN || errno == EWOULDBLOCK
#endif
        )) {
            std::cerr << "Error reading from socket" << std::endl;
            break;
        }
    }

    connection.close();
}