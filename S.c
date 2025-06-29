#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 9734

// Function to calculate Hamming(7,4) code
void hamming_encode(char data[5], char code[8]) {
    int d[4], p[3], c[7];

    for (int i = 0; i < 4; i++) {
        d[i] = data[i] - '0';
    }

    // Parity bits
    p[0] = d[0] ^ d[1] ^ d[3]; // p1
    p[1] = d[0] ^ d[2] ^ d[3]; // p2
    p[2] = d[1] ^ d[2] ^ d[3]; // p4

    // Place bits: p1 p2 d0 p4 d1 d2 d3
    c[0] = p[0];
    c[1] = p[1];
    c[2] = d[0];
    c[3] = p[2];
    c[4] = d[1];
    c[5] = d[2];
    c[6] = d[3];

    for (int i = 0; i < 7; i++) {
        code[i] = c[i] + '0';
    }
    code[7] = '\0';
}

int main() {
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char data[5], code[8];

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sockfd, 5);
    printf("Server started. Waiting for client...\n");

    while (1) {
        client_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);

        read(client_sockfd, data, sizeof(data));
        printf("Received data from client: %s\n", data);

        hamming_encode(data, code);
        printf("Sending Hamming codeword: %s\n", code);

        write(client_sockfd, code, strlen(code) + 1);
        close(client_sockfd);
    }

    close(server_sockfd);
    return 0;
}