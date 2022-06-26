int bpfCreateMap(enum bpf_map_type mapType, unsigned int keySize, unsigned int valueSize, unsigned int maxEntries)
{
	//参数 bpfMap的类型、Key大小、Valve大小、最大的键值对数目
	union bpf_attr attr = {
		.map_type = mapType,
		.key_size = keySize,
		.value_size = valueSize,
		.max_entries = maxEntries
	};

	return bpf(BPF_MAP_CREATE, &attr, sizeof(attr));
}

//默认宏方法BPF_HASH(name, keyType=u64, leafType=u64, size=10240)
//使用默认参数 keyType=u64 leafType=u64 size=10240
BPF_HASH(stats);
//使用自定义Key类型，保持默认，leafType=u64,size=10240
struct keyT
{
	char c[80];
};
BPF_HASH(counts, struct keyT);
//自定义所有参数
BPF_HASH(cpu_time, uint64_t, uint64_t, 4096)

