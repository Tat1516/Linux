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
    int sockfd = *(int *)arg;
    char bitstream[MAX_BITS];
    char divisor[MAX_BITS];
    char crc_result[MAX_BITS];
    char codeword[MAX_BITS * 2];
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    // Receive bitstream and divisor from client
    recvfrom(sockfd, bitstream, MAX_BITS, 0, (struct sockaddr*)&client_address, &client_len);
    recvfrom(sockfd, divisor, MAX_BITS, 0, (struct sockaddr*)&client_address, &client_len);

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
    sendto(sockfd, codeword, strlen(codeword) + 1, 0, (struct sockaddr*)&client_address, client_len);

    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_address;
    pthread_t th[MAXCLIENT];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(9734);

    bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));

    printf("CRC Server started (UDP). Listening on port 9734...\n");

    for (int i = 0; i < MAXCLIENT; i++) {
        pthread_create(&th[i], NULL, serve, &sockfd);
    }

    for (int i = 0; i < MAXCLIENT; i++) {
        pthread_join(th[i], NULL);
    }

    close(sockfd);
    return 0;
}
