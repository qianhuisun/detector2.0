#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
gcc demo.c -o demo
./demo
*/
typedef struct Sched_switch{
	char prev_comm[32];
	int prev_tid;
}Sched_switch;

typedef struct Syscall_entry_read{
	int fd;
	char filename[32];
}Syscall_entry_read;


typedef struct Test{
	char name[20];
	int num;
	union{
		Sched_switch sched;
		Syscall_entry_read read;
	}payload;
}Test;

int main(){
	Test test;
	memcpy(test.name, "100001", sizeof("100001"));
	test.num = 10;
	
	Syscall_entry_read read;
	read.fd = 11;
	memcpy(read.filename, "hello.c", sizeof("hello.c"));
	
	test.payload.read = read;
	printf("1%s\n", test.payload.read.filename);
	printf("2%s\n", test.payload.sched.prev_comm);

	Sched_switch s_switch;
	memcpy(s_switch.prev_comm, "cd", sizeof("cd"));
	s_switch.prev_tid=10;

	test.payload.sched = s_switch;
	printf("3%s\n", test.payload.read.filename);
	printf("4%s\n", test.payload.sched.prev_comm);
	return 0;
}