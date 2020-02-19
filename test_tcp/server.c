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

// https://stackoverflow.com/questions/25287551/passing-a-structure-with-array-of-integers-through-sockets-in-c
typedef struct UsrData{
        char usr_name[16];
        double address;
        int id;
} UsrData;

int main(int argc, char *argv[])
{

    UsrData recvUser;

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        // recv packet size
        int tmp, n;
        read(connfd, &tmp, sizeof(tmp));
        n = ntohl(tmp);
        printf("count = %d\n", n);

        // recv data
        UsrData* recvData;
        recvData = (UsrData*) malloc (n * sizeof(UsrData));
        recv(connfd, recvData, n * sizeof(UsrData), 0);
        printf("data 1 = %s\n", recvData[0].usr_name);
        printf("data 2 = %s\n", recvData[1].usr_name);

        close(connfd);
        sleep(1);
    }
}
