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
#include <inttypes.h>

typedef struct sched_switch{
	char prev_comm[32];
	int64_t prev_tid;
	int64_t prev_prio;
	int64_t prev_state;
	char next_comm[32];
	int64_t next_tid;
	int64_t next_prio;
} sched_switch;

typedef struct syscall_entry_read{
	uint64_t fd;
	uint64_t count;
} syscall_entry_read;

typedef struct syscall_exit_read{
	int64_t ret;
	uint64_t buf;
} syscall_exit_read;


typedef struct syscall_entry_write{
	uint64_t fd;
	uint64_t buf;
	uint64_t count;
} syscall_entry_write;

typedef struct syscall_exit_write{
	int64_t ret;
} syscall_exit_write;

typedef struct custom_event{
    bool last_event;
    char timestamp[32];
    char hostname[32];
    char domain[32];
    char event_name[32];
    uint64_t cpu_id;
    int64_t pid;
    union {
		sched_switch _sched_switch;
		syscall_entry_read _syscall_entry_read;
		syscall_exit_read _syscall_exit_read;
		syscall_entry_write _syscall_entry_write;
		syscall_exit_write _syscall_exit_write;
    } Payload;
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
    serv_addr.sin_port = htons(5022);

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
        if (strcmp(custom_event_object->event_name, "syscall_entry_write") == 0) {
            printf("%d\t event_name = %s cpu_id = %" PRIu64 " pid = %" PRIu64, i, custom_event_object->event_name, custom_event_object->cpu_id, custom_event_object->pid);
            // payload
            printf(" fd = %" PRIu64 " buf = %" PRIu64 " count = %" PRIu64 "\n", custom_event_object->Payload._syscall_entry_write.fd, custom_event_object->Payload._syscall_entry_write.buf, custom_event_object->Payload._syscall_entry_write.count);
        }
    }

    close(connfd);

    return 0;
}
