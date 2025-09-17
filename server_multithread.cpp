#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TCP_PORT 8080
#define UDP_PORT 9090
#define BUFFER_SIZE 1024

// Função para processar a requisição TCP em uma thread
void* handle_tcp_client(void* arg) {
    int client_socket = *(int*)arg;
    delete (int*)arg;
    char buffer[BUFFER_SIZE] = {0};

    int valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread > 0) {
        std::string data(buffer, valread);
        size_t comma_pos = data.find(',');

        if (comma_pos != std::string::npos) {
            std::string num1_str = data.substr(0, comma_pos);
            std::string num2_str = data.substr(comma_pos + 1);

            int num1 = std::stoi(num1_str);
            int num2 = std::stoi(num2_str);

            std::cout << "[TCP] Recebeu: " << num1 << " e " << num2 << ". Processando..." << std::endl;
            sleep(5);
            
            int sum = num1 + num2;
            std::string response = std::to_string(sum);
            
            send(client_socket, response.c_str(), response.length(), 0);
            std::cout << "[TCP] Resposta enviada: " << response << std::endl;
        }
    }
    close(client_socket);
    return NULL;
}

// Servidor TCP rodando em sua própria thread
void* start_tcp_server(void* arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket TCP creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind TCP failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("Listen TCP failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "[TCP] Servidor escutando na porta " << TCP_PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept TCP failed");
            continue;
        }
        int* p_new_socket = new int;
        *p_new_socket = new_socket;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_tcp_client, (void*)p_new_socket);
        pthread_detach(thread_id);
    }
    close(server_fd);
    return NULL;
}

// Servidor UDP
void start_udp_server() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket UDP creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind UDP failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "[UDP] Servidor escutando na porta " << UDP_PORT << std::endl;

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

            std::cout << "[UDP] Recebeu: " << num1 << " e " << num2 << ". Processando..." << std::endl;
            sleep(5);

            int sum = num1 + num2;
            std::string response = std::to_string(sum);
            sendto(sockfd, response.c_str(), response.length(), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            std::cout << "[UDP] Resposta enviada: " << response << std::endl;
        }
    }
    close(sockfd);
}

int main() {
    pthread_t tcp_thread;
    pthread_create(&tcp_thread, NULL, start_tcp_server, NULL);
    start_udp_server();
    pthread_join(tcp_thread, NULL);
    return 0;
}
