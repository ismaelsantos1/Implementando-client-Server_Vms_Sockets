#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define SERVER_HOST "192.168.56.101"
#define TCP_PORT 8080
#define UDP_PORT 9090
#define BUFFER_SIZE 1024

// Estrutura para passar dados para as threads
struct ThreadData {
    int id;
    int num1;
    int num2;
    int protocol; // 0 para TCP, 1 para UDP
};

void* run_client(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    if (data->protocol == 0) { // TCP
        int sock = 0;
        struct sockaddr_in serv_addr;
        char buffer[BUFFER_SIZE] = {0};

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cerr << "[Thread " << data->id << "] (TCP) Socket creation error" << std::endl;
            return NULL;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(TCP_PORT);
        if (inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0) {
            std::cerr << "[Thread " << data->id << "] (TCP) Invalid address" << std::endl;
            close(sock);
            return NULL;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "[Thread " << data->id << "] (TCP) Connection Failed" << std::endl;
            close(sock);
            return NULL;
        }

        std::string request = std::to_string(data->num1) + "," + std::to_string(data->num2);
        send(sock, request.c_str(), request.length(), 0);
        std::cout << "[Thread " << data->id << "] (TCP) Mensagem enviada: " << request << std::endl;

        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string response(buffer, valread);
            int received_sum = std::stoi(response);
            int correct_sum = data->num1 + data->num2;
            std::cout << "[Thread " << data->id << "] (TCP) Resposta recebida: " << received_sum << ". Verificação: " << (received_sum == correct_sum ? "Correta" : "Incorreta") << std::endl;
        }
        close(sock);
    } else { // UDP
        int sockfd;
        struct sockaddr_in servaddr;
        char buffer[BUFFER_SIZE];

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cerr << "[Thread " << data->id << "] (UDP) Socket creation error" << std::endl;
            return NULL;
        }
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(UDP_PORT);
        if (inet_pton(AF_INET, SERVER_HOST, &servaddr.sin_addr) <= 0) {
            std::cerr << "[Thread " << data->id << "] (UDP) Invalid address" << std::endl;
            close(sockfd);
            return NULL;
        }
        
        std::string request = std::to_string(data->num1) + "," + std::to_string(data->num2);
        sendto(sockfd, request.c_str(), request.length(), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        std::cout << "[Thread " << data->id << "] (UDP) Mensagem enviada: " << request << std::endl;

        socklen_t len;
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
        std::string response(buffer);
        int received_sum = std::stoi(response);
        int correct_sum = data->num1 + data->num2;
        std::cout << "[Thread " << data->id << "] (UDP) Resposta recebida: " << received_sum << ". Verificação: " << (received_sum == correct_sum ? "Correta" : "Incorreta") << std::endl;
        close(sockfd);
    }

    delete data;
    return NULL;
}

int main() {
    srand(time(0));
    std::vector<pthread_t> threads;
    
    for (int i = 0; i < 5; ++i) { // Cria 5 threads
        ThreadData* data = new ThreadData;
        data->id = i + 1;
        data->num1 = rand() % 100;
        data->num2 = rand() % 100;
        data->protocol = rand() % 2; // Alterna entre TCP e UDP

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, run_client, (void*)data);
        threads.push_back(thread_id);
    }

    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }

    std::cout << "Todas as threads foram executadas." << std::endl;
    return 0;
}
