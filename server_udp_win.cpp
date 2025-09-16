#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

void worker_process(SOCKET sock, sockaddr_in client, std::string payload) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "UDP datagram from " << client_ip << ":" << ntohs(client.sin_port)
              << " -> " << payload << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::string resp = "ACK from UDP server: " + payload;
    sendto(sock, resp.c_str(), resp.size(), 0, (sockaddr*)&client, sizeof(client));
}

int main(int argc, char** argv) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    int port = 6000;
    if (argc > 1) port = atoi(argv[1]);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(port);
    bind(sock, (sockaddr*)&serv, sizeof(serv));

    std::cout << "UDP server listening on port " << port << std::endl;

    while (true) {
        char buf[2048];
        sockaddr_in client{};
        int len = sizeof(client);
        int n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (sockaddr*)&client, &len);
        if (n > 0) {
            buf[n] = '\0';
            std::thread t(worker_process, sock, client, std::string(buf));
            t.detach();
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
