#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // Needed for strcpy

int main() {
int sockfd;
int len;
struct sockaddr_un address;
int result;
int stream[5], i;

// Create a socket
sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
if (sockfd == -1) {
perror("socket creation failed");
exit(1);
}

// Specify the address for the socket
address.sun_family = AF_UNIX;
strcpy(address.sun_path, "server_socket");
len = sizeof(address);

// Connect to the server
result = connect(sockfd, (struct sockaddr *)&address, len);

if (result == -1) {
perror("oops: client1");
close(sockfd); // Close the socket before exiting
exit(1);
}

// Input numbers from user
for(i = 0; i < 5; i++) {
printf("Enter a number: ");
scanf("%d", &stream[i]);
}

// Send data to the server
for(i = 0; i < 5; i++) {
write(sockfd, &stream[i], sizeof(int));
}

// Read data from the server
for(i = 0; i < 5; i++) {
read(sockfd, &stream[i], sizeof(int));
}

// Print received data from server
for(i = 0; i < 5; i++) {
printf("Received from server: %d\n", stream[i]);
}

// Close the socket connection
close(sockfd);

return 0;
}
