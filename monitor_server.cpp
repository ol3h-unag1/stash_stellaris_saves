#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Bind socket to file path
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);  // Ensure the path is clear
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Listen for connections
    if (listen(server_fd, 1) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Waiting for a connection...\n");
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept");
        exit(1);
    }

    printf("Connection established.\n");

    // Receive and echo message
    int n = read(client_fd, buffer, BUFFER_SIZE);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
