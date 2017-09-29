#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include "server.h"


pthread_mutex_t mutex;
int index_buffer;
char **buffer;

void *client_idle(void *thread)
{
    pthread_t pthread_self = *(pthread_t*)thread;
    while(1)
    {
        sleep(SLEEP_TIME);
        pthread_mutex_lock(&mutex);
        //printf("[%lu]: idle\n", pthread_self);
        buffer[index_buffer] = (char*)malloc(IDLE_MESSEGE_SIZE);
        sprintf(buffer[index_buffer], "[%lu]: idle", pthread_self);
        index_buffer++;
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}

void* client_serve(void* thread_data){
    struct thread_params *params = (struct readThreadParams *)thread_data;

    int sock = params->sock;
    char *client_ip = params->client_ip;
    //printf("[%lu]: accept new client %s\n",pthread_self(),client_ip);

    pthread_t idle;
    pthread_create(&idle, NULL, &client_idle, (void*)pthread_self());

    int bytes_read;
    char buf[BUFFER_SIZE];

    pthread_mutex_lock(&mutex);
    buffer[index_buffer] = (char*)malloc(ACCEPT_MESSEGE_SIZE+strlen(client_ip));
    sprintf(buffer[index_buffer], "[%lu]: accept new client %s", pthread_self(), client_ip);
    index_buffer++;
    pthread_mutex_unlock(&mutex);

    while(1)
    {
            bytes_read = recv(sock, buf, BUFFER_SIZE, 0);

            if(bytes_read > 0) {
                pthread_mutex_lock(&mutex);
                buffer[index_buffer] = (char*)malloc(strlen(buf)+1);
                sprintf(buffer[index_buffer], "%s",buf);
                //printf( "%s",buf);
                index_buffer++;
                pthread_mutex_unlock(&mutex);
                send(sock, buf, bytes_read, 0);
            }
            if(bytes_read == 0) {
                pthread_mutex_lock(&mutex);
                buffer[index_buffer] = (char*)malloc(DISCONNECT_MESSEGE_SIZE+strlen(client_ip));
                sprintf(buffer[index_buffer], "[%lu]: client  disconnected", pthread_self(), client_ip);
                index_buffer++;
                pthread_mutex_unlock(&mutex);
                close(sock);
                pthread_cancel(idle);
                break;
            }
            if(bytes_read == -1) perror("recv");


    }

    return 0;
}

void signal_handler()
{
    FILE *fl = fopen("temp.log", "w");
    if (fl == NULL){
       printf("Error create file");
       exit(-1);
    }
    for(int index = 0; index < index_buffer; index++){
        fprintf(fl, "%s\n", buffer[index]);
        free(buffer[index]);
    }
    free(buffer);
    fclose(fl);
    printf("temp.log");
    exit(0);
}


void start_server()
{
    int *sock, listener;
    struct sockaddr_in server_addr,client_addr;

    buffer = (char**) malloc((BUFFER_SIZE)*sizeof(char*));
    index_buffer = 0;
    signal(SIGINT, signal_handler);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);


    if (pthread_mutex_init(&mutex, NULL) != 0){
        printf("\n mutex init failed\n");
        return 1;
    }

    while(1)
    {
        sock = accept(listener, NULL, NULL);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip,sizeof(client_ip));

        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        pthread_t tid;
        struct thread_params read_params;
        read_params.client_ip = client_ip;
        read_params.sock = sock;

        pthread_create(&tid, NULL, client_serve, &read_params);

    }
    close(listener);
    return 0;
}
