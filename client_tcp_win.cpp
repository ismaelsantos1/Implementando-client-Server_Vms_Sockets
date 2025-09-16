#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: client_tcp <server_ip> <port> [message]\n";
        return 1;
    }

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);
    const char* msg = (argc >= 4) ? argv[3] : "Hello from TCP client";

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &serv.sin_addr);

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    send(sock, msg, strlen(msg), 0);

    char buf[2048];
    int n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        std::cout << "Response: " << buf << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
