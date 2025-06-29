#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_DATA_SIZE 4  // 4-bit dataword

/********************************* Hamming Code Functions ********************************/
void calculateHammingCode(int data[], int hammingCode[7]) {
    /* Data bits positions (0-based index):
       2 (d1), 4 (d2), 5 (d3), 6 (d4) */
    hammingCode[2] = data[0];  // d1
    hammingCode[4] = data[1];  // d2
    hammingCode[5] = data[2];  // d3
    hammingCode[6] = data[3];  // d4

    /* Calculate parity bits:
       p1 (pos 0) covers bits 0,2,4,6
       p2 (pos 1) covers bits 1,2,5,6
       p3 (pos 3) covers bits 3,4,5,6 */
    hammingCode[0] = hammingCode[2] ^ hammingCode[4] ^ hammingCode[6];
    hammingCode[1] = hammingCode[2] ^ hammingCode[5] ^ hammingCode[6];
    hammingCode[3] = hammingCode[4] ^ hammingCode[5] ^ hammingCode[6];
}

/************************************ Main Server Code ***********************************/
int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Configure server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server ready on port %d...\n", PORT);

    while(1) {  // Continuous operation
        int dataSize;
        socklen_t len = sizeof(cliaddr);
        
        // Receive data size
        ssize_t recv_len = recvfrom(sockfd, &dataSize, sizeof(int), 0,
                                   (struct sockaddr *)&cliaddr, &len);
        if(recv_len < 0) {
            perror("size receive error");
            continue;
        }

        // Validate data size
        if(dataSize != MAX_DATA_SIZE) {
            fprintf(stderr, "Invalid data size: %d\n", dataSize);
            continue;
        }

        int receivedData[MAX_DATA_SIZE];
        recv_len = recvfrom(sockfd, receivedData, MAX_DATA_SIZE * sizeof(int), 0,
                          (struct sockaddr *)&cliaddr, &len);
        if(recv_len < 0) {
            perror("data receive error");
            continue;
        }

        printf("Received dataword: ");
        for(int i=0; i<MAX_DATA_SIZE; i++) {
            printf("%d ", receivedData[i]);
        }
        printf("\n");

        // Calculate Hamming code
        int hammingCode[7] = {0};
        calculateHammingCode(receivedData, hammingCode);

        // Send response
        const int codeSize = 7;
        sendto(sockfd, &codeSize, sizeof(int), 0,
              (const struct sockaddr *)&cliaddr, len);
        sendto(sockfd, hammingCode, codeSize * sizeof(int), 0,
              (const struct sockaddr *)&cliaddr, len);

        printf("Sent codeword: ");
        for(int i=0; i<codeSize; i++) {
            printf("%d ", hammingCode[i]);
        }
        printf("\n\n");
    }

    close(sockfd);
    return 0;
}

