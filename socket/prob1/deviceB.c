#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "route_table.h"

#define DEVICE_ID "B"
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Coverse IP to binary
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // 3️⃣ connect to server (device A)
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }
    printf("[B] Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

    // create and send route table of B
    struct RouteTable tableB;
    init_table(&tableB, "B");
    print_table(&tableB);

    char sendbuf[1024];
    serialize_table(&tableB, sendbuf);
    send(sock, sendbuf, strlen(sendbuf), 0);
    printf("[B] Sent route table to A.\n");

    // receive route table from A
    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0)
    {
        printf("[B] Received route table from A: %s\n", buffer);

        struct RouteTable receivedTable;
        deserialize_table(buffer, &receivedTable);
        printf("\n[B] Parsed Route Table from A:\n");
        print_table(&receivedTable);
    }
    else
    {
        printf("[B] No data received from A.\n");
    }

    // close socket
    close(sock);
    printf("[B] Connection closed.\n");
    return 0;
}
