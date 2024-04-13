BPF_CALL_4(bpf_xdp_redirect_map, struct bpf_map *, map, u32, ifindex, u64, flags,
	   unsigned long, map_owner)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);

	if (unlikely(flags))
		return XDP_ABORTED;

	ri->ifindex = ifindex;
	ri->flags = flags;
	ri->map = map;
	ri->map_owner = map_owner;

	return XDP_REDIRECT;
}