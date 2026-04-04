#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

#define PORT 9090

void handle_client(int client_sock) {
    while (true) {
        char buffer[1024] = {0};

        int bytes = recv(client_sock, buffer, sizeof(buffer), 0);

        if (bytes <= 0) break;

        if (strcmp(buffer, "quit") == 0) break;

        send(client_sock, buffer, strlen(buffer), 0);
    }

    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Allow reuse of address/port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Setup address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen
    listen(server_fd, 5);

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        // Accept client
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        // Handle client in new thread
        std::thread t(handle_client, client_sock);
        t.detach();  // TRUE concurrency (Task 5)
    }

    close(server_fd);
    return 0;
}