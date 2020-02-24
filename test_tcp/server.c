#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>

typedef struct custom_event{
    bool last_event;
    char timestamp[32];
    char host_name[32];
    char domain[32];
    char event_name[32];
} custom_event;

int main(int argc, char *argv[])
{

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);
    
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    // recv data
    custom_event* custom_event_object;
    custom_event_object = (custom_event*) malloc (sizeof(custom_event));
    int i = 0;
    while (++i) {
        recv(connfd, custom_event_object, sizeof(custom_event), 0);
        if (custom_event_object->last_event) break;
        printf("%d\t event_name = %s\n", i, custom_event_object->event_name);
    }

    close(connfd);

    return 0;
}
