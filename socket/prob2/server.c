#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define DEVICE_ID "A"
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"
#define NUM_WORKERS 4
#define MAX_QUEUE 16

typedef struct
{
    int sockets[MAX_QUEUE];
    int front, rear;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} SocketQueue;

void queue_init(SocketQueue *q)
{
    q->front = q->rear = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_push(SocketQueue *q, int client_sock)
{
    pthread_mutex_lock(&q->lock);
    q->sockets[q->rear] = client_sock;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
}

int queue_pop(SocketQueue *q)
{
    pthread_mutex_lock(&q->lock);
    while (q->front == q->rear)
    { // queue empty
        pthread_cond_wait(&q->cond, &q->lock);
    }
    int sock = q->sockets[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    pthread_mutex_unlock(&q->lock);
    return sock;
}
typedef struct
{
    int id;             // the id of worker
    SocketQueue *queue; // pointer to the socket queue of this worker
} WorkerArgs;

void *worker_loop(void *arg)
{
    WorkerArgs *args = (WorkerArgs *)arg;
    int id = args->id;
    SocketQueue *q = args->queue;
    while (1)
    {
        int client_sock = queue_pop(q);
        if (client_sock < 0)
            continue;

        char buf[256];
        int n = read(client_sock, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = '\0';
            printf("[Worker %d] Received: %s\n", id, buf);
            write(client_sock, "OK\n", 3);
        }
        //close(client_sock);
    }
    return NULL;
}

int main()
{
    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server < 0)
    {
        perror("Socket creation failed");
        return -1;
    }
    printf("Socket created Successfully\n");

    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(socket_server);
        return -1;
    }

    printf("[A] Bound to 127.0.0.1:%d\n", SERVER_PORT);

    if (listen(socket_server, 10) < 0)
    {
        perror("listen failed");
        close(socket_server);
        return -1;
    }
    printf("[A] Listening for incoming connections...\n");

    pthread_t workers[NUM_WORKERS];
    SocketQueue queues[NUM_WORKERS];
    WorkerArgs args[NUM_WORKERS]; 

    for (int i = 0; i < NUM_WORKERS; ++i)
    {
        queue_init(&queues[i]);

        args[i].id = i;             
        args[i].queue = &queues[i]; 

        pthread_create(&workers[i], NULL, worker_loop, &args[i]);
        printf("[Server] Worker %d created.\n", i);
    }

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int next_worker = 0;

    while (1)
    {
        int client_sock = accept(socket_server, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("accept failed");
            continue;
        }
        printf("[Server] New client accepted\n");
        queue_push(&queues[next_worker], client_sock);
        next_worker = (next_worker + 1) % NUM_WORKERS;
    }

    return 0;
}
