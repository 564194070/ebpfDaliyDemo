#include<linux/sched.h>
#include<uapi/linux/ip.h>
#include<linux/inet.h>

//定义结构体 接收传递的参数
struct data_t {
	u32 pid;
	u64 ts;
	char comm[TASK_COMM_LEN];
	char strSAddr[16];
	char strDAddr[16];
	char name[20];
};

//定义性能事件映射
BPF_PERF_OUTPUT(events);

//HOOK函数 当内核触发系统调用ip时，触发我们的HOOK函数
int hookOutput(struct pt_regs *ctx, struct net *net, struct sock *sk, struct sk_buff *skb)
{
	// 定义的数据结构 准备从HOOK点拉数据
	struct data_t data = { };
	struct iphdr *iphdrOutput;
	// 辅助函数拉PID
	data.pid = bpf_get_current_pid_tgid();
	// 辅助函数拉时间
	data.ts = bpf_ktime_get_ns();
	memcpy(&data.name,"OutPut",7);
	//辅助函数拉进程名
	bpf_get_current_comm(&data.comm, sizeof(data.comm));
	

	/*{
	 	// 一些对参数的操作，还有点问题，后面修了再Update
		//辅助函数拉参数
		iphdrOutput = (struct iphdr *)(skb->head+ skb->network_header);
		//iphdrOutput.daddr = skb->network_header;
		//data.strSAddr 
		data.a= ntop(AF_INET, iphdrOutput->saddr);
		//data.strDAddr 
		data.b= ntop(AF_INET, iphdrOutput->daddr);
	        

                //data.a = inet_ntop(AF_INET, &data.strSAddr, iphdrOutput->saddr, sizeof(iphdrOutput->saddr)); 
		//data.b = inet_ntop(AF_INET, &data.strDAddr, iphdrOutput->daddr, sizeof(iphdrOutput->daddr)); 
		//}
	        //inet_ntop(AF_INET, iphdrOutput->saddr, data.strSAddr, sizeof(data.strSAddr));
                //inet_ntop(AF_INET, iphdrOutput->daddr, data.strDAddr, sizeof(data.strSAddr));	
	}*/	

	//提交性能事件
	events.perf_submit(ctx, &data, sizeof(data));
	return 0;
}
