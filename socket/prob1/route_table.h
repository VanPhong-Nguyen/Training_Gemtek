#ifndef ROUTE_TABLE_H
#define ROUTE_TABLE_H

struct RouteEntry {
    char destination[32];
    char next_hop[16];
    int metric;
};

struct RouteTable {
    struct RouteEntry entries[10];
    int count;
};

// Khai báo hàm
void init_table(struct RouteTable *t, const char *device_id);
void print_table(const struct RouteTable *t);
int serialize_table(const struct RouteTable *t, char *buffer);
void deserialize_table(const char *buffer, struct RouteTable *t);

#endif