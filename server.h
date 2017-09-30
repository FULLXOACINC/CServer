#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED
#include <pthread.h>
#define PORT_NUMBER 5223
#define BUFFER_SIZE 1024
#define SLEEP_TIME 1
#define ACCEPT_MESSEGE_SIZE 33
#define IDLE_MESSEGE_SIZE 19
#define DISCONNECT_MESSEGE_SIZE 35

struct thread_params {
    char* client_ip;
    int* sock;
};
pthread_mutex_t mutex;
int index_buffer;
char **buffer;
void start_server();
void *client_idle(void *thread);
void* client_serve(void* thread_data);
void signal_handler();
#endif // SERVER_H_INCLUDED
