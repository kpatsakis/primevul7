BPF_CALL_2(bpf_xdp_redirect, u32, ifindex, u64, flags)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);

	if (unlikely(flags))
		return XDP_ABORTED;

	ri->ifindex = ifindex;
	ri->flags = flags;
	ri->map = NULL;
	ri->map_owner = 0;

	return XDP_REDIRECT;
}