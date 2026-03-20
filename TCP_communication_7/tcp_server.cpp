#include <iostream>
#include <cstring> // memset
#include <unistd.h> // close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_addr
#include <cstdio> // perror

const int PORT  = 5050;

int main() {
 int server_fd = socket(AF_INET, SOCK_STREAM, 0);
 if (server_fd < 0) {
 perror("socket");
 return 1;
 }
 int opt = 1;
 setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 sockaddr_in addr{};
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = htons(PORT);
 if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
 perror("bind");
 close(server_fd);
 return 1;
 }
 if (listen(server_fd, 1) < 0) {
 perror("listen");
 close(server_fd);
 return 1;
 }
 std::cout << "TCP server listening on port: " << PORT << std::endl;
 sockaddr_in client_addr{};
 socklen_t client_len = sizeof(client_addr);
 int client_fd = accept(server_fd, (struct sockaddr*)&client_addr,
&client_len);
 if (client_fd < 0) {
 perror("accept");
 close(server_fd);
 return 1;
 }
 std::cout << "Client connected." << std::endl;
 char buffer[256];
 while (true) {
 std::memset(buffer, 0, sizeof(buffer));
 ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
 if (n <= 0) {
 std::cout << "Client disconnected or error." << std::endl;
 break;
 }
 std::cout << "Server received: " << buffer << std::endl;
 // Echo back
 send(client_fd, buffer, n, 0);
 }
 close(client_fd);
 close(server_fd);
 return 0;
}