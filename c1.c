#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8760
#define BUFFER_SIZE 1024

int sock;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void *recv_thread(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int n = read(sock, buffer, BUFFER_SIZE - 1);
        if (n <= 0) {
            pthread_mutex_lock(&print_lock);
            printf("\nServer disconnected.\n");
            pthread_mutex_unlock(&print_lock);
            exit(0);
        }
        buffer[n] = '\0';
        pthread_mutex_lock(&print_lock);
        printf("\rServer: %s", buffer);
        printf("Client: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    pthread_t tid;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");

    pthread_create(&tid, NULL, recv_thread, NULL);

    char buffer[BUFFER_SIZE];
    while (1) {
        pthread_mutex_lock(&print_lock);
        printf("Client: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break;
        
        pthread_mutex_lock(&print_lock);
        write(sock, buffer, strlen(buffer));
        pthread_mutex_unlock(&print_lock);
    }

    close(sock);
    return 0;
}
