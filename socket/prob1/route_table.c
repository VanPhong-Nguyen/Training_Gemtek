// route_table.c
#include <stdio.h>
#include <string.h>
#include "route_table.h"

// Khởi tạo route table ban đầu cho thiết bị
void init_table(struct RouteTable *t, const char *device_id)
{
    t->count = 0;

    if (strcmp(device_id, "A") == 0) {
        strcpy(t->entries[0].destination, "10.0.1.0/24");
        strcpy(t->entries[0].next_hop, "-");
        t->entries[0].metric = 0;
        t->count = 1;
    } else if (strcmp(device_id, "B") == 0) {
        strcpy(t->entries[0].destination, "10.0.2.0/24");
        strcpy(t->entries[0].next_hop, "-");
        t->entries[0].metric = 0;
        t->count = 1;
    }
}

// In bảng định tuyến ra màn hình
void print_table(const struct RouteTable *t)
{
    printf("---- Route Table ----\n");
    printf("%-15s %-10s %-6s\n", "Destination", "NextHop", "Metric");
    for (int i = 0; i < t->count; i++) {
        printf("%-15s %-10s %-6d\n",
               t->entries[i].destination,
               t->entries[i].next_hop,
               t->entries[i].metric);
    }
    printf("----------------------\n");
}

// Chuyển route table sang chuỗi để gửi qua socket
int serialize_table(const struct RouteTable *t, char *buffer)
{
    char line[128];
    buffer[0] = '\0'; // clear

    for (int i = 0; i < t->count; i++) {
        sprintf(line, "%s,%s,%d;", 
                t->entries[i].destination,
                t->entries[i].next_hop,
                t->entries[i].metric);
        strcat(buffer, line);
    }
    return strlen(buffer);
}

// Đọc chuỗi nhận được và chuyển lại thành route table
void deserialize_table(const char *buffer, struct RouteTable *t)
{
    t->count = 0;
    char temp[1024];
    strcpy(temp, buffer);

    char *entry = strtok(temp, ";");
    while (entry != NULL && t->count < 10) {
        sscanf(entry, "%[^,],%[^,],%d",
               t->entries[t->count].destination,
               t->entries[t->count].next_hop,
               &t->entries[t->count].metric);
        t->count++;
        entry = strtok(NULL, ";");
    }
}
