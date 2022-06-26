#emmmmmmmmmmmm 自己写的中文的ebpf学习路径,菜狗一枚，咕噜咕噜



## 00.前言
1.本文参考项目: 极客时间《eBPF核心技术与实战》(我觉得是好文章，安利给大家)


## 00.简介内容
eBPF介绍：
1.eBPF经过JIT编译后直接在CPU运行，最好叫作BPF执行引擎而不是内核虚拟机
2.eBPF主要运用于故障诊断，网络优化，安全控制，性能监控等领域
3.主要作用，允许非内核开发者去拓展内核功能
4.Linux 2.1.75首次引入BPF；3.0解释器被即时编译器取代；包过滤直接在内核执行，避免向用户态复制(疑惑点)
  2014年 eBPF开始拓展BPF:增加寄存器，引入BPF存储，从只有数据包过滤发展到内核态/用户态函数、追踪点，性能事件，安全控制等能力。
5.无需修改内核源码和重新编译就能拓展内核功能

eBPF设计：
1.内核态引入了新的虚拟机，所有指令都在内核虚拟机中运行
2.用户态使用BPF字节码定义过滤表达式，传递给内核，内核执行
3.执行过程分为以下几步：1.LLVM将eBPF装化成BPF字节码；
                        2.bpf系统调用提交给内核，内核接收后验证器校验字节码，通过就执行。
                        3.下HOOK(kprobe,uprobe,tracepoints,perf_events)
                        4.返回触发的性能事件
                        5.BPF映射存储数据存储数据
eBPF学习路径:
1.编程接口：程序类型、BPF钩子，BPFMap，辅助函数，BPF/libbpf,CO-RE
2.应用实践：1.动态追踪：bcc，bpftrace
            2.观测监控: Pixie，Hubble，kubectl-trace
            3.网络：Cilium，Calico，Katran
            4.安全：Falco，Tracee


eBPF工作:
1.HOOK时间触发后执行(系统调用、内核跟踪点、内核函数、用户态函数、网路事件)
2.插桩内核态(kprobe)；插桩用户态(uprobe)

# 01.安装工具及测试程序编写
安装工具:
1.将eBPF程序编译成字节码的LLVM
2.C语言编译工具make
3.eBPF工具集和BCC和它依赖的头文件
4.内核代码仓库实时同步的libbpf
5.内核代码提供的eBPF管理工具bpftool
命令：sudo apt-get install -y  make clang llvm libelf-dev libbpf-dev bpfcc-tools libbpfcc-dev linux-tools-$(uname -r) linux-headers-$(uname -r)

测试程序，测试netfilter中iptable中OUTPUT的HOOK点

# 02.简单的使用使用性能事件优化程序01
1.加入了简单的辅助函数,详见READMEFunDesc.md
2.使用BPF映射和性能事件对程序进行了优化，不在内核中打印信息，使用BPF映射在用户态和内核态交互。
3.BPF向内存中存储数据，用户通过BPF映射获取数据。从映射中读取内核eBPF程序运行状态。


