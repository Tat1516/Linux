#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    int i, n, stream[200];
    socklen_t len = sizeof(server_addr);

    // Get user input
    printf("Enter number of bits: ");
    scanf("%d", &n);
    printf("Enter binary stream (%d bits):\n", n);
    for(i = 0; i < n; i++) {
        scanf("%d", &stream[i]);
        if(stream[i] != 0 && stream[i] != 1) {
            printf("Error: Only 0/1 allowed\n");
            exit(1);
        }
    }

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
        perror("Socket error");
        exit(1);
    }

    // Server setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9734);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send data
    sendto(sockfd, &n, sizeof(int), 0, (struct sockaddr*)&server_addr, len);
    for(i = 0; i < n; i++) {
        if(sendto(sockfd, &stream[i], sizeof(int), 0, (struct sockaddr*)&server_addr, len) == -1) {
            perror("Send error");
            close(sockfd);
            exit(1);
        }
    }

    // Receive response
    int m;
    recvfrom(sockfd, &m, sizeof(int), 0, (struct sockaddr*)&server_addr, &len);
    printf("\nStuffed stream (%d bits):\n", m);
    for(i = 0; i < m; i++) {
        if(recvfrom(sockfd, &stream[i], sizeof(int), 0, (struct sockaddr*)&server_addr, &len) == -1) {
            perror("Receive error");
            close(sockfd);
            exit(1);
        }
        printf("%d ", stream[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}
