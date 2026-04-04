#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9090

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    std::cout << "Connected to server. Type messages (type 'quit' to exit)\n";

    while (true) {
        std::string message;
        std::getline(std::cin, message);

        send(sock, message.c_str(), message.length(), 0);

        if (message == "quit") break;

        int bytes = recv(sock, buffer, sizeof(buffer), 0);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }
    }

    close(sock);
    return 0;
}