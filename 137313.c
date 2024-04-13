void xdp_do_flush_map(void)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);
	struct bpf_map *map = ri->map_to_flush;

	ri->map_to_flush = NULL;
	if (map) {
		switch (map->map_type) {
		case BPF_MAP_TYPE_DEVMAP:
			__dev_map_flush(map);
			break;
		case BPF_MAP_TYPE_CPUMAP:
			__cpu_map_flush(map);
			break;
		default:
			break;
		}
	}
}