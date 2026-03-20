#include <iostream>
#include <cstring> // memset
#include <unistd.h> // close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio> // perror

const int PORT = 5050;

int main() {
 int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
 if (sock_fd < 0) {
 perror("socket");
 return 1;
 }
 sockaddr_in server_addr{};
 server_addr.sin_family = AF_INET;
 server_addr.sin_port = htons(PORT);
 server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))
< 0) {
 perror("connect");
 close(sock_fd);
 return 1;
 }
 std::cout << "Connected to TCP server." << std::endl;
 const char* messages[] = {
 "Hello from TCP client 1",
 "This is another message",
 "Goodbye"
 };
 char buffer[256];
 for (const char* msg : messages) {
 ssize_t n = send(sock_fd, msg, std::strlen(msg), 0);
 if (n < 0) {
 perror("send");
 break;
 }
 std::memset(buffer, 0, sizeof(buffer));
 n = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
 if (n <= 0) {
 std::cout << "Server closed connection or error." << std::endl;
 break;
 }
 std::cout << "Client received echo: " << buffer << std::endl;
 sleep(1);
 }
 close(sock_fd);
 return 0;
}