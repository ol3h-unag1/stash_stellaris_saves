#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <cstring>

// Writes the specified string to a Unix domain socket
void write_to_socket(const std::string& what, const std::string& socket_path) {
    int sock;
    struct sockaddr_un addr;

    // Create a Unix domain socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

    // Set up the socket address structure
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (socket_path.size() >= sizeof(addr.sun_path)) {
        close(sock);
        throw std::runtime_error("socket path too long");
    }
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path));

    // Connect to the socket
    if (connect(sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(sock);
        throw std::runtime_error("connect failed: " + std::string(strerror(errno)));
    }

    // Write the message to the socket
    if (write(sock, what.c_str(), what.size()) == -1) {
        close(sock);
        throw std::runtime_error("write failed: " + std::string(strerror(errno)));
    }

    // Close the socket
    close(sock);
}

// Reads a string from a Unix domain socket
std::string read_from_socket(const std::string& socket_path) {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    constexpr size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    // Create a Unix domain socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

    // Set up the socket address structure
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (socket_path.size() >= sizeof(addr.sun_path)) {
        close(server_fd);
        throw std::runtime_error("socket path too long");
    }
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path));

    // Bind the socket to the file path
    unlink(socket_path.c_str());
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(server_fd);
        throw std::runtime_error("bind failed: " + std::string(strerror(errno)));
    }

    // Listen for a connection
    if (listen(server_fd, 1) == -1) {
        close(server_fd);
        unlink(socket_path.c_str());
        throw std::runtime_error("listen failed: " + std::string(strerror(errno)));
    }

    // Accept a client connection
    if ((client_fd = accept(server_fd, nullptr, nullptr)) == -1) {
        close(server_fd);
        unlink(socket_path.c_str());
        throw std::runtime_error("accept failed: " + std::string(strerror(errno)));
    }

    // Read the message from the client
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read == -1) {
        close(client_fd);
        close(server_fd);
        unlink(socket_path.c_str());
        throw std::runtime_error("read failed: " + std::string(strerror(errno)));
    }

    // Null-terminate the received data
    buffer[bytes_read] = '\0';
    std::string received_message(buffer);

    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(socket_path.c_str());

    return received_message;
}
