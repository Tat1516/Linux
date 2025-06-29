#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address, client_address;
    int stream[200], stuffed[400];  // Increased buffer size for safety
    
    // Create socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sockfd == -1) {
        perror("Socket error");
        exit(1);
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);
    
    // Bind socket
    if(bind(server_sockfd, (struct sockaddr*)&server_address, server_len) == -1) {
        perror("Bind error");
        close(server_sockfd);
        exit(1);
    }

    // Listen for connections
    if(listen(server_sockfd, 5) == -1) {
        perror("Listen error");
        close(server_sockfd);
        exit(1);
    }
    printf("Server ready\n");

    while(1) {
        printf("\n Server Waiting\n");
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, 
                             (struct sockaddr*)&client_address,
                             &client_len);
        if(client_sockfd == -1) {
            perror("Accept error");
            continue;
        }

        // Receive data length
        int original_len;
        if(read(client_sockfd, &original_len, sizeof(int)) <= 0) {
            perror("Length read error");
            close(client_sockfd);
            continue;
        }

        // Validate input length
        if(original_len <= 0 || original_len > 200) {
            printf("Invalid length: %d\n", original_len);
            close(client_sockfd);
            continue;
        }

        // Receive data stream
        for(int i = 0; i < original_len; i++) {
            if(read(client_sockfd, &stream[i], sizeof(int)) <= 0) {
                perror("Data read error");
                close(client_sockfd);
                continue;
            }
        }

        // Bit stuffing logic
        int new_len = 0;
        int tracking = 0;  // 1 when we're looking for five 1s after a 0
        int one_count = 0;
        
        for(int i = 0; i < original_len; i++) {
            // Always add current bit first
            stuffed[new_len++] = stream[i];
            
            if(tracking) {
                if(stream[i] == 1) {
                    one_count++;
                    if(one_count == 5) {
                        // Insert stuffed 0 after five 1s
                        stuffed[new_len++] = 0;
                        tracking = 0;
                        one_count = 0;
                    }
                } else {
                    // Reset tracking on new 0
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
        if(write(client_sockfd, &new_len, sizeof(int)) == -1) {
            perror("Length write error");
            close(client_sockfd);
            continue;
        }
        
        for(int i = 0; i < new_len; i++) {
            if(write(client_sockfd, &stuffed[i], sizeof(int)) == -1) {
                perror("Data write error");
                break;
            }
        }

        close(client_sockfd);
        printf("Processed %d → %d bits\n", original_len, new_len);
    }

    close(server_sockfd);
    return 0;
}

