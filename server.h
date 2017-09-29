#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

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
void start_server();
#endif // SERVER_H_INCLUDED
