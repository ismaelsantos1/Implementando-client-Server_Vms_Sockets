#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char const* argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converte o endereço IP
    if (inet_pton(AF_INET, "192.168.56.101", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/Address not supported" << std::endl;
        return -1;
    }

    // Conexão ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return -1;
    }

    int num1 = 10, num2 = 20;
    std::string request = std::to_string(num1) + "," + std::to_string(num2);

    send(sock, request.c_str(), request.length(), 0);
    std::cout << "[TCP] Mensagem enviada: " << request << std::endl;

    // Recebe a resposta
    int valread = read(sock, buffer, BUFFER_SIZE);
    if (valread > 0) {
        std::string response(buffer, valread);
        int received_sum = std::stoi(response);
        int correct_sum = num1 + num2;

        std::cout << "[TCP] Resposta do servidor: " << received_sum << std::endl;
        if (received_sum == correct_sum) {
            std::cout << "[TCP] Verificação correta: " << num1 << " + " << num2 << " = " << correct_sum << std::endl;
        } else {
            std::cout << "[TCP] Verificação incorreta! Esperado: " << correct_sum << ", Recebido: " << received_sum << std::endl;
        }
    }

    close(sock);
    return 0;
}
