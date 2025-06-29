#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int sockfd;
    struct sockaddr_in address;
    int result;
    int i, n, stream[200];
    socklen_t len;

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

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("Socket error");
        exit(1);
    }

    // Server setup
    address.sin_family = AF_INET;
    address.sin_port = htons(9734);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    len = sizeof(address);

    // Connect
    result = connect(sockfd, (struct sockaddr*)&address, len);
    if(result == -1) {
        perror("Connect error");
        close(sockfd);
        exit(1);
    }

    // Send data
    write(sockfd, &n, sizeof(int));  // Send length first
    for(i = 0; i < n; i++) {
        if(write(sockfd, &stream[i], sizeof(int)) == -1) {
            perror("Write error");
            close(sockfd);
            exit(1);
        }
    }

    // Receive response
    int m;
    read(sockfd, &m, sizeof(int));  // Get new length
    printf("\nStuffed stream (%d bits):\n", m);
    for(i = 0; i < m; i++) {
        if(read(sockfd, &stream[i], sizeof(int)) == -1) {
            perror("Read error");
            close(sockfd);
            exit(1);
        }
        printf("%d ", stream[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}

