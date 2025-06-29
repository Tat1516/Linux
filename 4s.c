#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXCLIENT 3
#define MAX_BITS 100

int server_sockfd;

void compute_crc(char *bitstream, char *divisor, char *crc_result) {
    int len_bitstream = strlen(bitstream);
    int len_divisor = strlen(divisor);

    // Append zeros (divisor length - 1)
    char padded[MAX_BITS];
    strcpy(padded, bitstream);
    for (int i = 0; i < len_divisor - 1; i++) {
        strcat(padded, "0");
    }

    // Perform division (XOR)
    for (int i = 0; i <= strlen(padded) - len_divisor; ) {
        if (padded[i] == '1') {
            for (int j = 0; j < len_divisor; j++) {
                padded[i + j] = (padded[i + j] == divisor[j]) ? '0' : '1';
            }
        } else {
            i++;
        }
    }

    // Extract CRC (last 'len_divisor - 1' bits)
    strncpy(crc_result, padded + len_bitstream, len_divisor - 1);
    crc_result[len_divisor - 1] = '\0';
}

void *serve(void *arg) {
    char bitstream[MAX_BITS];
    char divisor[MAX_BITS];
    char crc_result[MAX_BITS];
    char codeword[MAX_BITS * 2];  // Original data + CRC

    int client_sockfd, client_len;
    struct sockaddr_in client_address;

    printf("Server waiting for client...\n");
    client_len = sizeof(client_address);
    client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);

    // Receive bitstream and divisor from client
    read(client_sockfd, bitstream, MAX_BITS);
    read(client_sockfd, divisor, MAX_BITS);

    printf("Received bitstream: %s\n", bitstream);
    printf("Received divisor: %s\n", divisor);

    // Compute CRC
    compute_crc(bitstream, divisor, crc_result);

    // Construct codeword (original data + CRC)
    strcpy(codeword, bitstream);
    strcat(codeword, crc_result);

    printf("Computed CRC: %s\n", crc_result);
    printf("Final codeword: %s\n", codeword);

    // Send back the full codeword (data + CRC)
    write(client_sockfd, codeword, strlen(codeword) + 1);

    close(client_sockfd);
    return NULL;
}

int main() {
    int server_len;
    struct sockaddr_in server_address;
    pthread_t th[MAXCLIENT];

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);
    listen(server_sockfd, MAXCLIENT);

    printf("CRC Server started. Listening on port 9734...\n");

    for (int i = 0; i < MAXCLIENT; i++) {
        pthread_create(&th[i], NULL, serve, NULL);
    }

    for (int i = 0; i < MAXCLIENT; i++) {
        pthread_join(th[i], NULL);
    }

    close(server_sockfd);
    return 0;
    }
