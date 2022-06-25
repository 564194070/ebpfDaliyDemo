#!/user/bin/env python3

#加载bcc的库
from bcc import BPF

#加载我们写的C源代码，之后被编译成BPF字节码
byte = BPF(src_file="HookOutput.c")

#将BPF程序挂载到内核探针kprobe,其中ip_local_out是iptablesOutput的系统调用
byte.attach_kprobe(event="ip_local_out",fn_name="hookOutput")

#读取内核调试文件 /sys/kernel/debug/tracing/trace_pipe
byte.trace_print()
