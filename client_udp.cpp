#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main(int argc, char const* argv[]) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "192.168.56.101", &servaddr.sin_addr) <= 0) {
        std::cout << "Invalid address/Address not supported" << std::endl;
        return -1;
    }

    int num1 = 5, num2 = 15;
    std::string request = std::to_string(num1) + "," + std::to_string(num2);

    sendto(sockfd, request.c_str(), request.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    std::cout << "[UDP] Mensagem enviada: " << request << std::endl;

    socklen_t len;
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    std::string response(buffer);

    int received_sum = std::stoi(response);
    int correct_sum = num1 + num2;

    std::cout << "[UDP] Resposta do servidor: " << received_sum << std::endl;
    if (received_sum == correct_sum) {
        std::cout << "[UDP] Verificação correta: " << num1 << " + " << num2 << " = " << correct_sum << std::endl;
    } else {
        std::cout << "[UDP] Verificação incorreta! Esperado: " << correct_sum << ", Recebido: " << received_sum << std::endl;
    }

    close(sockfd);
    return 0;
}
