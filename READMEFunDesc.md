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

py.
BPF(src_file=""):加载开发的C源代码
attach_kprobe(event="",fn_name=""):将程序中的函数挂载到内核探针
trace_print:读取内核调试文件的内容
b["eventName"].open_perf_buffer(func()):传入回调函数，处理Perf事件类型的BPF映射中读取的数据
                               (func (cpu,data,size))
b.perf_buffer_poll():是否有性能事件发生

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


strace -v -f -ebpf ./x.py：查看给bpf函数传入的参数
			   1.表示加载BPF程序，
			   2.bpf_attr结构体(BPF程序的类型，指令条数，具体指令，程序名称)
			   3.表示属性的大小





kernel
include/uapi/linux/bpf.h bpf_cmd 内核已经支持的BPF命令，辅助函数，映射类型
vmlinux.h 获取内核数据结构定义，避免找不到头文件和数据结构
