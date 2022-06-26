# 加载bcc库
from bcc import BPF

# 加载源文件
b = BPF(src_file="HookOutput.c")
b.attach_kprobe(event="ip_local_out", fn_name="hookOutput")

# 打印一个头出来，方便观察打印数据的意义
print("%-18s %-16s %-6s %-16s" % ("TIME(s)", "COMM", "PID","Called"))

# 回调之后要对数据进行处理，打印性能事件提供的数据
start = 0
def printEvent(cpu, data, size):
    global start
    event = b["events"].event(data)
    if start == 0:
            start = event.ts
    time_s = (float(event.ts - start)) / 1000000000
    print("%-18.9f %-16s %-6d %-16s" % (time_s, event.comm, event.pid, event.name))

# eventsd的事件映射到这里，通过循环读取映射的内容，执行回调(print_event)输出内容
b["events"].open_perf_buffer(printEvent)
while 1:
    try:
        b.perf_buffer_poll()
    except KeyboardInterrupt:
        exit()
