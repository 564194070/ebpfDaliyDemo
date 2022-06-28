# 任务目标：监控系统中的短时进程
# 执行思路：监控进程的启动，观察创建的进程以及是否有进程重复创建(短时不好监控，自身崩溃称其，间隔较短) 
# 解决问题：工具一般按照时间采样，不会实时采集信息。
# 问题研究：进程启动的流程 主进程fork 子进程execve

"""
解决过程：
1.查询相关追踪点 bpftrace -l '*execve*'
2.选择跟踪点 bpftrace -l '*tracepoint:syscalls*:*execve*'
3.查看出入口具体参数 bpftrace -lv tracepoint:syscalls:sys_enter_execve
"""

构建bpftrace程序
bpftrace -e 表示直接从字符串中读入bpftrace程序
','用于分割多个追踪点
printf() 向终端打印
pid comm 内置变量 进程PID和进程名称
join(args->argv) 把字符串格式的参数用空格拼接起来，打印到终端

bpftrace -e 'tracepoint:syscalls:sys_enter_execve,tracepoint:syscalls:sys_enter_execveat { printf("%-6d %-8s", pid, comm); join(args->argv);}'

