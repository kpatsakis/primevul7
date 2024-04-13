static int bpf_check_tail_call(const struct bpf_prog *fp)
{
	struct bpf_prog_aux *aux = fp->aux;
	int i;

	for (i = 0; i < aux->used_map_cnt; i++) {
		struct bpf_map *map = aux->used_maps[i];
		struct bpf_array *array;

		if (map->map_type != BPF_MAP_TYPE_PROG_ARRAY)
			continue;

		array = container_of(map, struct bpf_array, map);
		if (!bpf_prog_array_compatible(array, fp))
			return -EINVAL;
	}

	return 0;
}