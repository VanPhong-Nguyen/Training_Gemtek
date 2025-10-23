#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>   // IP header
#include <netinet/udp.h>  // UDP header

#define BUF_SIZE 65536

int main() {
    int sock_raw;
    char buffer[BUF_SIZE];

    // Create a raw socket to capture all IP packets
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw < 0) {
        perror("Socket creation failed");
        return 1;
    }

    printf("[Server] Raw socket created. Listening for packets...\n");

    while (1) {
        ssize_t data_size = recv(sock_raw, buffer, BUF_SIZE, 0);
        if (data_size < 0) {
            perror("recv failed");
            close(sock_raw);
            return 1;
        }

        struct iphdr *ip = (struct iphdr *)buffer;
        struct in_addr src, dst;
        src.s_addr = ip->saddr;
        dst.s_addr = ip->daddr;

        printf("\n===== New Packet =====\n");
        printf("From: %s\n", inet_ntoa(src));
        printf("To:   %s\n", inet_ntoa(dst));
        printf("Protocol: %d\n", ip->protocol);
        printf("Total Length: %d bytes\n", ntohs(ip->tot_len));
    }

    close(sock_raw);
    return 0;
}
