#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BITS 100

int main() {
    int sockfd;
    struct sockaddr_in server_address;
    char bitstream[MAX_BITS];
    char divisor[MAX_BITS];
    char codeword[MAX_BITS * 2];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(9734);

    printf("Enter bitstream (e.g., 101110): ");
    scanf("%s", bitstream);

    printf("Enter divisor (e.g., 1011): ");
    scanf("%s", divisor);

    // Send bitstream and divisor to server
    sendto(sockfd, bitstream, MAX_BITS, 0, (struct sockaddr*)&server_address, sizeof(server_address));
    sendto(sockfd, divisor, MAX_BITS, 0, (struct sockaddr*)&server_address, sizeof(server_address));

    // Receive the full codeword (data + CRC)
    recvfrom(sockfd, codeword, MAX_BITS * 2, 0, NULL, NULL);

    printf("Received codeword (data + CRC): %s\n", codeword);

    close(sockfd);
    return 0;
}
