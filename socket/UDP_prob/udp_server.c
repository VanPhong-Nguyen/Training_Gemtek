#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5000
#define MAX_BUFFER 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER];
    socklen_t client_len = sizeof(client_addr);

    // Create UDP socket (Datagram socket)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;      // Bind to all interfaces
    server_addr.sin_port = htons(SERVER_PORT);     // Server port

    // Bind socket to IP and port
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server started on port %d\n", SERVER_PORT);

    while (1) {
        // Receive message from any client
        int n = recvfrom(sockfd, buffer, MAX_BUFFER - 1, 0,
                         (struct sockaddr*)&client_addr, &client_len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0'; // Null-terminate the message
        printf("[Client %s:%d] says: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        // Send response to client
        const char *reply = "Message received!";
        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr*)&client_addr, client_len);
    }

    close(sockfd);
    return 0;
}
