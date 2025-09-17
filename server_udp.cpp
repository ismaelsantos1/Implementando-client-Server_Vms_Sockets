#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[UDP] Servidor escutando na porta " << PORT << std::endl;

    socklen_t len;
    while (true) {
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        buffer[n] = '\0';
        std::string data(buffer);
        size_t comma_pos = data.find(',');

        if (comma_pos != std::string::npos) {
            std::string num1_str = data.substr(0, comma_pos);
            std::string num2_str = data.substr(comma_pos + 1);

            int num1 = std::stoi(num1_str);
            int num2 = std::stoi(num2_str);

            std::cout << "[UDP] Recebeu a requisição de soma: " << num1 << " e " << num2 << ". Processando..." << std::endl;
            sleep(5); // Simula um atraso

            int sum = num1 + num2;
            std::string response = std::to_string(sum);
            
            sendto(sockfd, response.c_str(), response.length(), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            std::cout << "[UDP] Resposta enviada: " << response << std::endl;
        }
    }

    close(sockfd);
    return 0;
}
