#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // Include for strcpy

int main() {
int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_un server_address, client_address;

// Remove any old socket. This is important!
unlink("server_socket");

// Create a socket
server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
if (server_sockfd == -1) {
perror("socket");
exit(EXIT_FAILURE);
}

// Name the socket (bind it to an address)
server_address.sun_family = AF_UNIX;
strcpy(server_address.sun_path, "server_socket");
server_len = sizeof(server_address);

if (bind(server_sockfd, (struct sockaddr *)&server_address, server_len) == -1) {
perror("bind");
close(server_sockfd); // Close the socket before exiting
exit(EXIT_FAILURE);
}

// Create a connection queue and wait for clients
if (listen(server_sockfd, 5) == -1) {
perror("listen");
close(server_sockfd);
exit(EXIT_FAILURE);
}

while (1) {
int stream[5];
int i, j;

printf("server waiting\n");

client_len = sizeof(client_address);
client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t*)&client_len); // Corrected accept
if (client_sockfd == -1) {
perror("accept");
continue; // Continue to the next iteration to accept new connections
}

// Read data from the client
for (i = 0; i < 5; i++) {
if (read(client_sockfd, &stream[i], sizeof(int)) == -1) {
perror("read");
close(client_sockfd);
continue; // Handle read error and continue with the next client
}
}

// Sort the data
for (i = 0; i < 5; i++) {
for (j = i + 1; j < 5; j++) {
if (stream[i] < stream[j]) {
int a = stream[i];
stream[i] = stream[j];
stream[j] = a;
}
}
}

// Write the sorted data back to the client
for (i = 0; i < 5; i++) {
if (write(client_sockfd, &stream[i], sizeof(int)) == -1) {
perror("write");
close(client_sockfd);
continue; // Handle write error and continue with the next client
}
}

// Close the client socket
close(client_sockfd);
}

// The following line will never execute because the `while(1)` loop is infinite.
// close(server_sockfd); // you probably don't want to close the server socket

return 0;
}
