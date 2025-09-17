#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>

#define PORT 8080
#define BUFFER_SIZE 1024

// Função para processar a requisição de um cliente em uma thread
void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    delete (int*)arg;
    char buffer[BUFFER_SIZE] = {0};

    // Recebe os dados do cliente
    int valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread > 0) {
        std::string data(buffer, valread);
        size_t comma_pos = data.find(',');

        if (comma_pos != std::string::npos) {
            std::string num1_str = data.substr(0, comma_pos);
            std::string num2_str = data.substr(comma_pos + 1);

            int num1 = std::stoi(num1_str);
            int num2 = std::stoi(num2_str);

            std::cout << "[TCP] Recebeu a requisição de soma: " << num1 << " e " << num2 << ". Processando..." << std::endl;
            sleep(5); // Simula um atraso

            int sum = num1 + num2;
            std::string response = std::to_string(sum);
            
            // Envia a resposta
            send(client_socket, response.c_str(), response.length(), 0);
            std::cout << "[TCP] Resposta enviada: " << response << std::endl;
        }
    }

    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Criação do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associação do socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Escuta por conexões
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[TCP] Servidor escutando na porta " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        // Aloca o socket em um ponteiro para a thread
        int* p_new_socket = new int;
        *p_new_socket = new_socket;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void*)p_new_socket);
        pthread_detach(thread_id); // Não espera o término da thread

        std::cout << "[TCP] Nova conexão. Total de threads ativas: " << pthread_getconcurrency() << std::endl;
    }

    close(server_fd);
    return 0;
}
