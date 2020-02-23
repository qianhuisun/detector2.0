typedef struct event_object{
	// ref to print_timestamp
	int64_t timestamp; 
	char hostname[32];
	char domain[32];
	char event_name[32];
	int cpu_id;
	int t_id;
	// solution 1-->讲payload内容序列化，尽可能的传输，不丢失数据
	// 格式如"comm:lttng_consumerd;tid:4946;prio:20;target_cpu:3"
	char payload_str[256];
	// solution 2-->struct + union + struct
	// 但是需要针对每种类型的event做特定的填值处理
	// 接受方也需要针对event类型进行处理
	union{
		Sched_switch sched;
		Syscall_entry_read sys_entry_read;
		Syscall_exit_read  sys_exit_read;
		Syscall_entry_write sys_entry_write;
		Syscall_exit_write sys_exit_write;
	}payload;

}event_object;


typedef struct Sched_switch{
	char prev_comm[32];
	int prev_tid;
	int prev_prio;
	int prev_state;
	char next_comm[32];
	int next_tid;
	int next_prio;
}Sched_switch;

typedef struct Syscall_entry_read{
	int fd;
	double count;
}Syscall_entry_read;

typedef struct Syscall_exit_read{
	int ret;
	double buf;
}Syscall_exit_read;


typedef struct Syscall_entry_write{
	int fd;
	double buf;
	int count;
}Syscall_entry_write;

typedef struct Syscall_exit_write{
	int ret;
}Syscall_exit_write;







