# eBPF工具/辅助函数/类型统计表

# 辅助函数
c.
void bpf_trace_printk(string): 在内核调试文件打印信息(sys/kernel/debug/tracing/trace_pipe)
u32 bpf_get_current_pid_tgid():获取PID信息
u64 bpf_ktime_get_ns():获取系统启动之后过了的时间
bool bpf_get_current_comm(&dstVar, sizeof(dstVar)):获取进程名称，复制到缓冲区
//read有安全检查，禁止缺页中断发生
void bpf_probe_read(&dstVar, sizeof(dstVar),(void * )srcVar):将参数提取出来，从指针处读固定大小的数据
void bpf_probe_read_str(&dstVar, sizeof(dstVar),(void * )srcVar):将参数提取出来，从指针处读出字符串
bpf_get_current_task()：获取当前任务的task结构体
bpf_perf_event_output(ctx,map,flags,data,size)：向性能事件缓冲区写数据
bpf_get_stackld(ctx,map,flags)：获取内核态和用户态调用栈
BPF_PERF_OUTPUT(eventsName):定义性能映射，定义一个Perf时间类型的BPF映射)
eventsName.perf_submit(ctx, &CRDStruct, sizeof(CRD)):提交性能事件

BPF
union bpf_attr attr = {}
attr.target_fd = target_fd //cgroup 文件描述符
attr.attach_bpf_fd = prog_fd //BPF程序描述符号
attr.attach_type = BPF_CGROUP_DEVICE //挂在类型为BPF_CGROUP_DEVICE

bpf(BPF_PROG_ATTACH,&attr,sizeof(attr)) < 0

py.
BPF(src_file=""):加载开发的C源代码
attach_kprobe(event="",fn_name=""):将程序中的函数挂载到内核探针
trace_print:读取内核调试文件的内容
b["eventName"].open_perf_buffer(func()):传入回调函数，处理Perf事件类型的BPF映射中读取的数据
                               (func (cpu,data,size))
b.perf_buffer_poll():是否有性能事件发生

b = BPF(src_file="xdp-example.c")
fn = b.load_func("xdp-example", BPF.XDP) 编译XDP程序
device = "eth0"
b.attach_xdp(device, fn, 0) 加载程序到网卡
b.remove_xdp(device) 卸载程序
b.attach_uretprobe() 挂载用户态程序
b.attach_uretprobe(name="/usr/bin/bash", sym="readline", fn_name="bash_readline")

sudo tc qdisc add dev eth0 clsact 创建 clsact 类型的排队规则
sudo tc filter add dev eth0 ingress bpf da obj tc-example.o sec ingress 加载接收方向的 eBPF 程序
sudo tc filter add dev eth0 egress bpf da obj tc-example.o sec egress 加载发送方向的 eBPF 程序


tools:
bpftool:查看eBPF程序的运行状态
bpftool prog list：查询系统中正在运行的eBPF程序(编号，类型，名称)
bpftool prog dump xlated id eBPF编号:查看指令(BPF指令行数，BPF指令码，BPF指令伪代码)
bpftool prog dump jited id 89:BPF指令到内核后，即时编译器生成本地机器指令，执行。
bpftool feature probe:查询当前系统支持的辅助函数列表
bpftool map：查询系统中所有的映射
bpftool map create /sys/fs/bpf/stats_map type hash key 2 value 2 entries 8 name statusMap:挂Map(KV都是2字节)8元素叫statsMap
bpftool map update name statsMap key 0xc1 0xc2 value 0xa1 0xa2：向哈希表中插入数据
bpftool map dump name statsMap：查询数据表中的所有数据i
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h：方便bpf_probe_read时，提取到正确的数据类型
rm /sys/fs/bpf/statsMap：删除哈希映射表
bpftool map dump id diname
bpftool feature probe | grep program_type:查询系统支持的程序类型

strace -v -f -ebpf ./x.py：查看给bpf函数传入的参数
			   1.表示加载BPF程序，
			   2.bpf_attr结构体(BPF程序的类型，指令条数，具体指令，程序名称)
			   3.表示属性的大小





kernel
include/uapi/linux/bpf.h bpf_cmd 内核已经支持的BPF命令，辅助函数，映射类型
vmlinux.h 获取内核数据结构定义，避免找不到头文件和数据结构



跟踪eBPF程序
BPF_PROG_TYPE_KPROBE 特定函数动态插桩，内核态krpobe，用户态uprobe
BPF_PROG_TYPE_TRACEPOINT 内核静态跟踪点 perf list
BPF_PROG_TYPE_PERF_EVENT 性能事件跟踪，内核调用。定时器，硬件数据
网络eBPF程序
XDP BPF_PROG_TYPE_XDP 网络驱动程序刚刚收到数据包时触发 没有绕过协议栈，只是在协议栈前处理，处理完去协议栈
	1.XDP程序加载到网卡 sudo ip link set dev eth1 xdpgeneric object xdp-example.o
	2.XDP程序卸载 sudo ip link set veth1 xdpgeneric off 
TC BPF_PROG_TYPE_SCHED_CLS(流量控制的分类器) 和 BPF_PROG_TYPE_SCHED_ACT（流量控制执行器 ）
	1.通过网卡队列、排队规则、分类器、过滤器以及执行器等，实现了对网络流量的整形调度和带宽控制
	2.直接获取内核解析后的网络报文数据结构
	3.在网卡的接收和发送两个方向上执行
	4.发送时处理，数据包发送到网卡队列(GSO)之前,iptables和IP层处理之后
	5.接收时处理，数据包发送到网卡队列(GSO)之后,iptables和IP层处理之前
套接字 用于过滤、观测或重定向套接字网络包
	1.可以挂载到套接字（socket）、控制组（cgroup ）以及网络命名空间（netns）
cgroup
	1.对 cgroup 内所有进程的网络过滤、套接字选项以及转发等进行动态控制
	2.典型的应用场景是对容器中运行的多个进程进行网络控制
