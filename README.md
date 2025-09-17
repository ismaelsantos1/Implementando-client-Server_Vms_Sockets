# Trabalho de Sistemas Distribuídos – Unidade I

## 📌 Descrição
Este projeto foi desenvolvido para a disciplina de **Sistemas Distribuídos** e tem como objetivo implementar comunicação entre **clientes e servidores** utilizando **sockets TCP e UDP**, bem como o uso de **threads** para atender múltiplos clientes simultaneamente.  

Foram implementados dois cenários principais:

- **Cenário 1:**  
  - Três máquinas virtuais (1 servidor + 2 clientes).  
  - Comunicação via **sockets TCP e UDP**.  
  - O servidor deve atender dois clientes **ao mesmo tempo** (utilizando `sleep` para simular processamento).  

- **Cenário 2:**  
  - Duas máquinas virtuais (1 servidor + 1 cliente).  
  - Comunicação via **sockets TCP e UDP**.  
  - Clientes são **multithreads**, podendo rodar várias instâncias ao mesmo tempo.  
  - O servidor deve ser capaz de atender todos os clientes concorrentes.  

---

## ⚙️ Tecnologias utilizadas
- **Linguagem:** C++  
- **Ambiente:** Windows (compilador MinGW + g++)  
- **Virtualização:** VirtualBox  
- **Threads:** biblioteca `<thread>`  
- **Sockets:** biblioteca `<winsock2.h>`  

---

## 📂 Estrutura do Projeto
/Trabalho-Sistemas-Distribuidos
│
├── README.md
│
├── Cenario1
│ ├── servidor_tcp.cpp
│ ├── servidor_udp.cpp
│ ├── cliente_tcp.cpp
│ └── cliente_udp.cpp
│
└── Cenario2
├── servidor_tcp.cpp
├── servidor_udp.cpp
├── cliente_tcp_multithread.cpp
└── cliente_udp_multithread.cpp


---


🖥️ Ambiente de Virtualização

Utilizamos o VirtualBox para simular os cenários:

Cenário 1: 1 VM servidor + 2 VMs clientes.

Cenário 2: 1 VM servidor + 1 VM cliente.

As máquinas foram clonadas para facilitar a configuração do ambiente.

O sistema operacional pode ser Windows ou Linux (mas este projeto está configurado para Linux).

📚 Aprendizados

Uso de sockets TCP e UDP no Windows com Winsock2.

Implementação de threads em C++.

Experiência prática com máquinas virtuais e comunicação em rede.

Simulação de concorrência entre clientes e servidor.
