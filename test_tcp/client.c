#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
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

    ////////////data preparation////////////////
    custom_event custom_event_object1;
    custom_event_object1.last_event = false;
    memcpy(custom_event_object1.timestamp, "1234567", sizeof("1234567"));
    memcpy(custom_event_object1.host_name, "1234567", sizeof("1234567"));
    memcpy(custom_event_object1.domain, "1234567", sizeof("1234567"));
    memcpy(custom_event_object1.event_name, "1", sizeof("1"));

    custom_event custom_event_object2;
    custom_event_object2.last_event = false;
    memcpy(custom_event_object2.timestamp, "1234567", sizeof("1234567"));
    memcpy(custom_event_object2.host_name, "1234567", sizeof("1234567"));
    memcpy(custom_event_object2.domain, "1234567", sizeof("1234567"));
    memcpy(custom_event_object2.event_name, "2", sizeof("2"));

    custom_event custom_event_object3;
    custom_event_object3.last_event = false;
    memcpy(custom_event_object3.timestamp, "1234567", sizeof("1234567"));
    memcpy(custom_event_object3.host_name, "1234567", sizeof("1234567"));
    memcpy(custom_event_object3.domain, "1234567", sizeof("1234567"));
    memcpy(custom_event_object3.event_name, "3", sizeof("3"));

    custom_event custom_event_object4;
    custom_event_object4.last_event = true;
    memcpy(custom_event_object4.timestamp, "1234567", sizeof("1234567"));
    memcpy(custom_event_object4.host_name, "1234567", sizeof("1234567"));
    memcpy(custom_event_object4.domain, "1234567", sizeof("1234567"));
    memcpy(custom_event_object4.event_name, "4", sizeof("4"));

    int count = 4;

    ////need duplicately save data///
    custom_event* custom_event_objects;
    custom_event_objects = (custom_event*) malloc (count * sizeof(custom_event));
    memcpy(&custom_event_objects[0], &custom_event_object1, sizeof(custom_event));
    memcpy(&custom_event_objects[1], &custom_event_object2, sizeof(custom_event));
    memcpy(&custom_event_objects[2], &custom_event_object3, sizeof(custom_event));
    memcpy(&custom_event_objects[3], &custom_event_object4, sizeof(custom_event));

    ////////////////////////

    int sockfd = 0;
    struct sockaddr_in serv_addr;
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    printf("data 1 = %s\n", custom_event_objects[0].event_name);
    send(sockfd, &custom_event_objects[0], sizeof(custom_event), 0);
    printf("data 2 = %s\n", custom_event_objects[1].event_name);
    send(sockfd, &custom_event_objects[1], sizeof(custom_event), 0);
    printf("data 3 = %s\n", custom_event_objects[2].event_name);
    send(sockfd, &custom_event_objects[2], sizeof(custom_event), 0);
    printf("data 4 = %s\n", custom_event_objects[3].event_name);
    send(sockfd, &custom_event_objects[3], sizeof(custom_event), 0);

    return 0;
}