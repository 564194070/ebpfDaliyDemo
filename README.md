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

# 03.eBPF的工作原理和流程
1.eBPF虚拟机和虚拟化虚拟机不同：后者基于x86/arm64通用指令集，可以完成所有计算机功能；前者指令集有限。
2.eBPF指令采用了C调用约定，提供的辅助函数可以在C语言中直接调用。
3.eBPF在内核中运行时主要的五个模块：
	1.eBPF辅助函数，eBPF程序和内核其他模块交互的函数，不同的函数集不同的eBPF函数可调用
	2.eBPF验证其，确保安全，指令变为有向无环图，不允许出现不可达指令，无效指令
	3.11个64位寄存器，1个程序计数器，512字节的存储模块：R0存调用和返回，R1-R5存参数，R10只读存栈
	4.即时编译器：eBPF字节码编译成本地机器指令，内核执行
	5.BPF映射：可以被用户进程访问，进而控制eBPF程序的状态
4.eBPF程序的执行过程：
	BCC执行ebpf程序，BPF指令加载和执行的过程
	1.C源码被编译成BPF字节码
	2.BCC负责eBPF程序的编译和加载过程，借助bpf系统调用加载BPF程序，返回文件描述符
	3.BCC开展系统调用
	4.程序加载到内核
	5.查询事件类型，/sys/bus/event_source/devices查询的
	6.将程序和具体的内核调用事件绑定，perf_event_open将程序和内核跟踪/用户跟踪等事件绑定，事件
	7.iocatl + PERF_EVENT_IOC_SET_BPF绑定BPF到事件
	8.内核性能监控模块在内核事件发生时，自动执行我们的eBPF程序
5.eBPF程序结构：
	1.用户态：eBPF程序的加载，事件绑定，eBPF程序运行结果汇总输出，通过bpf系统调用交互内核
	2.内核态：运行在虚拟机中，负责定制和控制系统的运行状态

# 04.如何创建一个BPFMap和关于它的简单使用
1.BPF映射：
        1.eBPF需要大块存储，不能如同常规内核代码去分配内存，要通过BPF映射完成
        2.提供了大块键值存储，eBPF最多可以访问64个BPF映射，多个eBPF可以共享BPF映射共享状态
        3.BPF程序和BPFMap交互(KV)(内核态交互)，用户态进程从BPF(内核)拉数据分析
        4.大量系统调用和辅助函数都是访问BPF映射的
	5.BPF辅助函数中并没有BPF映射的创建函数，BPF映射只能通过用户态程序的系统调用来创建
2.BPF问题：
	1.eBPF中BPF辅助函数中并没有BPF映射的创建函数，BPF映射只能通过用户态程序的系统调用来创建。
	2.eBPF中BPF不能删除映射函数，在用户态程序关闭文件描述符后自动删除。要保留就用BPF_OBJ_PIN() 挂/sys/fs/bpf
3.内核数据结构:
	1.需要引入内核文件才能用数据结构
	2.路径和版本不同，会发生错误引用事件
	3.生产环境不允许安装内核头文件。
	4.使用vmlinux.h解决问题，和调试信息。
	5.CO-RE：对不同内核访问偏移量改写；在libpbf中预定义不同内核版本中的数据结构。

# 05.关于eBPF程序触发机制和应用场景
1.eBPF程序分类：
	1.bpf_prog_type规定了内核支持的不同类型的eBPF程序：
		1.跟踪，从内核和程序的运行状态中提取跟踪信息，系统正在干啥,为监控排错优化，提供数据支持
		2.网络，网络数据包过滤和处理，管理数据包收发过程，网络观测，过滤，性能优化
		3.安全控制，BPF拓展。
		4.eBPF程序可以通过BPF映射互相配合

# 06.关于查询内核中的跟踪点
1.查询跟踪点
	1.内核查询 cat /proc/kallsyms | wc -l 不建议使用 /sys/kernel/debug/
	2.bpftrace查询 构建了简化跟踪语言 .bt可使用
		1.查询搜友内核插装和跟踪点 boftrace -l  bpftrace -l "*execve*"
		2.查询入口参数和返回值格式 bpftrace -lv tracepoint:syscalls:sys_enter(exit)_execve

# 07.如何开发一个解决短时进程的监控程序
1.开发eBPF程序的方式：
	1.bpftrace：快速排查和定位系统，单行脚本快速开发排查。不支持复杂程序依赖BCC和LLVM
	2.BCC 开发复杂eBPF程序中，内置工具多。依赖LLVM和内核头文件才可以动态编译加载
	3.libbpf从内核中抽离标准库，开发的eBPF程序可以直接分发执行，不需要机器安装LLVM和内核头文件。 但是要系统开启BTF特性。

# 08.使用eBPF监控用户态进程
1.方法
	1.把内核态跟踪使用的kprobe和tracepoint替换成uprobe,或者用户空间定义的静态跟踪点(USTD)
	2.-g + 调试信息 readelf/objdump,nm 
	3.readelf -Ws /usr/lib/x86_64-linux-gnv/lib.so.6 readelf -n /usr/lib/x86_64-linux-gnu/libc.so.6
	4.bpftrace bpftrace -l 'uprobe:/usr/lib/x86_64-linux-gnu/libc.so.6:*' bpftrace -l 'usdt:/usr/lib/x86_64-linux-gnu/libc.so.6:*'
	5.插桩基于文件，不过滤进程PID之前，默认所有用到的函数都要插桩
	6.编译形语言可以，解释性要追踪解析器函数，即时编译型语言超级难
2.过程：
	1.sudo apt install bash-dbgsym 安装调试信息
	2.readelf -n /usr/bin/bash | grep 'Build ID' 查ID
	3.ls /usr/lib/debug/.build-id/7b/140b33fd79d0861f831bae38a0cdfdf639d8bc.debug 找到对应的文件
	4.readelf -Ws /usr/lib/debug/.build-id/7b/140b33fd79d0861f831bae38a0cdfdf639d8bc.debug 查符号表	
