static void *__xdp_map_lookup_elem(struct bpf_map *map, u32 index)
{
	switch (map->map_type) {
	case BPF_MAP_TYPE_DEVMAP:
		return __dev_map_lookup_elem(map, index);
	case BPF_MAP_TYPE_CPUMAP:
		return __cpu_map_lookup_elem(map, index);
	default:
		return NULL;
	}
}