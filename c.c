#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_DATA_SIZE 32  // Maximum allowed data size

int main() {
    int sockfd;
    char buffer[MAX_DATA_SIZE];
    struct sockaddr_in servaddr;
    
    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    
    // Configure server address (now points to localhost)
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Explicit localhost

    int n = 4;
    int arr[MAX_DATA_SIZE];

    printf("Enter 4-bit dataword (0s and 1s): ");
    for (int i = 0; i < n; i++) {
        scanf("%1d", &arr[i]);  // %1d ensures single digit input
        if(arr[i] != 0 && arr[i] != 1) {
            fprintf(stderr, "Error: Only 0 or 1 allowed\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    // Send data
    sendto(sockfd, &n, sizeof(int), 0, 
          (const struct sockaddr *)&servaddr, sizeof(servaddr));
    sendto(sockfd, arr, n * sizeof(int), 0,
          (const struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Data sent. Waiting for response...\n");

    // Receive response with proper error checking
    int size;
    socklen_t len = sizeof(servaddr);  // Correct type and initialization
    
    ssize_t recv_len = recvfrom(sockfd, &size, sizeof(int), 0,
                              (struct sockaddr *)&servaddr, &len);
    if(recv_len < 0) {
        perror("size receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Validate received size
    if(size > MAX_DATA_SIZE || size < 1) {
        fprintf(stderr, "Invalid size received: %d\n", size);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int receivedBits[size];
    recv_len = recvfrom(sockfd, receivedBits, size * sizeof(int), 0,
                      (struct sockaddr *)&servaddr, &len);
    if(recv_len < 0) {
        perror("data receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Received codeword: ");
    for(int i = 0; i < size; i++) {
        printf("%d ", receivedBits[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}

