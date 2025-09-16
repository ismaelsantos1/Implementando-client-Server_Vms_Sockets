#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

void handle_client(SOCKET client_fd, sockaddr_in client_addr) {
    char buf[1024];
    int n = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Received from " << client_ip << ":" << ntohs(client_addr.sin_port)
                  << " -> " << buf << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::string resp = "ACK from TCP server: received '" + std::string(buf) + "'";
        send(client_fd, resp.c_str(), resp.size(), 0);
    }
    closesocket(client_fd);
}

int main(int argc, char** argv) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    int port = 5000;
    if (argc > 1) port = atoi(argv[1]);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(port);
    bind(sock, (sockaddr*)&serv, sizeof(serv));
    listen(sock, 10);

    std::cout << "TCP server listening on port " << port << std::endl;

    while (true) {
        sockaddr_in client{};
        int len = sizeof(client);
        SOCKET client_fd = accept(sock, (sockaddr*)&client, &len);
        if (client_fd != INVALID_SOCKET) {
            std::thread t(handle_client, client_fd, client);
            t.detach();
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
