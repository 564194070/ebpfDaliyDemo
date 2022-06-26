# eBPF工具/辅助函数/类型统计表

# 辅助函数
c.
void bpf_trace_printk(string): 在内核调试文件打印信息(sys/kernel/debug/tracing/trace_pipe)
u32 bpf_get_current_pid_tgid():获取PID信息
u64 bpf_ktime_get_ns():获取系统启动之后过了的时间
bool bpf_get_current_comm(&dstVar, sizeof(dstVar)):获取进程名称，复制到缓冲区
void bpf_probe_read(&dstVar, sizeof(dstVar),(void * )srcVar):将参数提取出来，从指针出读固定大小的数据

BPF_PERF_OUTPUT(eventsName):定义性能映射，定义一个Perf时间类型的BPF映射)
eventsName.perf_submit(ctx, &CRDStruct, sizeof(CRD)):提交性能事件

py.
BPF(src_file=""):加载开发的C源代码
attach_kprobe(event="",fn_name=""):将程序中的函数挂载到内核探针
trace_print:读取内核调试文件的内容
b["eventName"].open_perf_buffer(func()):传入回调函数，处理Perf事件类型的BPF映射中读取的数据
                               (func (cpu,data,size))
b.perf_buffer_poll():是否有性能事件发生



