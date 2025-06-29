#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9734

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char data[5], code[8];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Enter 4-bit data word (e.g., 1011): ");
    scanf("%4s", data);

    write(sockfd, data, strlen(data) + 1);
    read(sockfd, code, sizeof(code));

    printf("Received Hamming codeword from server: %s\n", code);

    close(sockfd);
    return 0;
}