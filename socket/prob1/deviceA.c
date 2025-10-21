#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "route_table.h"

#define DEVICE_ID "A"
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

int main()
{
    // create socket
    int socket_server;
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server < 0)
    {
        perror("Socket creation failed");
        return -1;
    }
    printf("Socket created successfully\n");
    // config server address
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP global
    server_addr.sin_port = htons(5000);                   // port 5000
                                                          // bind socket with specific address and port
    if (bind(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(socket_server);
        return -1;
    }
    printf("[A] Bound to 127.0.0.1:5000.\n");

    if (listen(socket_server, 1) < 0)
    {
        perror("listen failed");
        close(socket_server);
        return -1;
    }
    printf("[A] Listening for incoming connections...\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_socket = accept(socket_server, (struct sockaddr *)&client_addr, &client_len);

    if (new_socket < 0)
    {
        perror("accept failed");
        close(socket_server);
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");
    read(new_socket, buffer, sizeof(buffer));
    printf("Client says: %s\n", buffer);

    // response mess to client
    struct RouteTable tableA;
    init_table(&tableA, "A");
    print_table(&tableA);

    char sendbuf[1024];
    serialize_table(&tableA, sendbuf);
    send(new_socket, sendbuf, strlen(sendbuf), 0);
    printf("Message sent to client.\n");

    // close socket
    close(new_socket);
    return 0;
}