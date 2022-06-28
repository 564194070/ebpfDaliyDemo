#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <linux/fs.h>

//定义参数长度和参数个数常量 栈空间只有512B
#define ARGSIZE 64
#define TOTALMAXARGS 5
#define FULLMAXARGSARR (TOTALMAXARGS * ARGSIZE)
#define LAST_ARG (FULLMAXARGSARR - ARGSIZE)


// 写一个BPF映射用到的拿数据的数据结构 包含了进程的基本信息
struct dataMap
{
	u32 pid;
	char comm[TASK_COMM_LEN];
	int retval;
	unsigned int args_size;
	char argv[FULLMAXARGSARR];
};

static int __bpf_read_arg_str(struct dataMap *data, const char *ptr);
//定义性能事件，实时返回Hook函数的结果
BPF_PERF_OUTPUT(events);
BPF_HASH(tasks,u32,struct dataMap);

//定义跟踪点函数
//入口跟踪点逻辑
TRACEPOINT_PROBE(syscalls, sys_enter_execve)
{
    	// 变量定义    
    	unsigned int ret = 0;    
	const char **argv = (const char **)(args->argv);

	struct dataMap data = {};
	u32 pid = bpf_get_current_pid_tgid();
	data.pid = pid;
	bpf_get_current_comm(&data.comm, sizeof(data.comm));

	//捞参数，可执行文件名字
	if (__bpf_read_arg_str(&data, (const char *)argv[0]) < 0)
	{        
		goto out;    
	}
	
	// 获取其他参数（限定最多5个）    
#pragma unroll
	for (int i = 1; i < TOTALMAXARGS; i++) 
	{        
		if (__bpf_read_arg_str(&data, (const char *)argv[i]) < 0) 
		{            
			goto out;        
		}    
	}

	out:
	// store the data in hash map
	tasks.update(&pid, &data);
	return 0;
}

//出口耿总点点理解
TRACEPOINT_PROBE(syscalls,sys_exit_execve)
{
	// 从哈希映射中查询进程基本信息
	u32 pid = bpf_get_current_pid_tgid();
	struct dataMap *data = tasks.lookup(&pid);

	//填充返回值并提交到性能事件映射中
	if (data != NULL) 
	{        
		data->retval = args->ret;        
		events.perf_submit(args, data, sizeof(struct dataMap));        
		// 最后清理进程信息        
		tasks.delete(&pid);    
	}    
	return 0;
}





// 从用户空间读取字符串
static int __bpf_read_arg_str(struct dataMap *data, const char *ptr)
{
    if (data->args_size > LAST_ARG) {
        return -1;
    }

    int ret = bpf_probe_read_user_str(&data->argv[data->args_size], ARGSIZE, (void *)ptr);
    if (ret > ARGSIZE || ret < 0) {
        return -1;
    }

    // increase the args size. the first tailing '\0' is not counted and hence it
    // would be overwritten by the next call.
    data->args_size += (ret - 1);

    return 0;
}
