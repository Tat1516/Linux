#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int sockfd;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    int stream[200], stuffed[400];
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
        perror("Socket error");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(9734);
    
    // Bind socket
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind error");
        close(sockfd);
        exit(1);
    }

    printf("UDP Server ready\n");

    while(1) {
        printf("\nServer Waiting\n");
        client_len = sizeof(client_addr);

        // Receive data length
        int original_len;
        if(recvfrom(sockfd, &original_len, sizeof(int), 0, (struct sockaddr*)&client_addr, &client_len) <= 0) {
            perror("Length receive error");
            continue;
        }

        // Validate input length
        if(original_len <= 0 || original_len > 200) {
            printf("Invalid length: %d\n", original_len);
            continue;
        }

        // Receive data stream
        for(int i = 0; i < original_len; i++) {
            if(recvfrom(sockfd, &stream[i], sizeof(int), 0, (struct sockaddr*)&client_addr, &client_len) <= 0) {
                perror("Data receive error");
                continue;
            }
        }

        // Bit stuffing logic
        int new_len = 0;
        int tracking = 0;
        int one_count = 0;
        
        for(int i = 0; i < original_len; i++) {
            stuffed[new_len++] = stream[i];
            
            if(tracking) {
                if(stream[i] == 1) {
                    one_count++;
                    if(one_count == 5) {
                        stuffed[new_len++] = 0;
                        tracking = 0;
                        one_count = 0;
                    }
                } else {
                    tracking = (stream[i] == 0) ? 1 : 0;
                    one_count = 0;
                }
            } else {
                if(stream[i] == 0) {
                    tracking = 1;
                    one_count = 0;
                }
            }
        }

        // Send back stuffed data
        if(sendto(sockfd, &new_len, sizeof(int), 0, (struct sockaddr*)&client_addr, client_len) == -1) {
            perror("Length send error");
            continue;
        }
        
        for(int i = 0; i < new_len; i++) {
            if(sendto(sockfd, &stuffed[i], sizeof(int), 0, (struct sockaddr*)&client_addr, client_len) == -1) {
                perror("Data send error");
                break;
            }
        }

        printf("Processed %d → %d bits\n", original_len, new_len);
    }

    close(sockfd);
    return 0;
}
