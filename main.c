#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>





void* threadFunc(void* thread_data){
    int sock =(int) thread_data;

    printf("SOCK %i\n",sock);
    int bytes_read;
    char buf[1024];

    while(1)
    {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            printf("SOCK %i SEND:\n",sock);
            printf(buf);
            printf("\n");
            send(sock, buf, bytes_read, 0);

    }
    printf("SOCK %i CLOSE\n",sock);

    close(sock);

    return NULL;
}

int main()
{
    int *sock, listener;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5223);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while(1)
    {
        printf("GOOD\n");
        sock = accept(listener, NULL, NULL);
        //printf("SOCK %i\n",sock);

        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        pthread_t tid;

        pthread_create(&tid, NULL, threadFunc, sock);

        printf("End\n");
        //close(sock);
    }
    close(listener);
    return 0;
}
