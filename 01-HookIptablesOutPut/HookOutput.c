int hookOutput (void *ctx)
{
	bpf_trace_printk("Output be called");
	return 0;
}
