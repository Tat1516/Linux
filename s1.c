#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8760
#define BUFFER_SIZE 1024

int client_fd;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void *recv_thread(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int n = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (n <= 0) {
            pthread_mutex_lock(&print_lock);
            printf("\nClient disconnected.\n");
            pthread_mutex_unlock(&print_lock);
            exit(0);
        }
        buffer[n] = '\0';
        pthread_mutex_lock(&print_lock);
        printf("\rClient: %s", buffer);
        printf("Server: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Chatroom Ready!\nWaiting for client...\n");

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client Connected!\n");

    pthread_create(&tid, NULL, recv_thread, NULL);

    char buffer[BUFFER_SIZE];
    while (1) {
        pthread_mutex_lock(&print_lock);
        printf("Server: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break;
        
        pthread_mutex_lock(&print_lock);
        write(client_fd, buffer, strlen(buffer));
        pthread_mutex_unlock(&print_lock);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
