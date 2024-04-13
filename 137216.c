BPF_CALL_2(bpf_redirect, u32, ifindex, u64, flags)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);

	if (unlikely(flags & ~(BPF_F_INGRESS)))
		return TC_ACT_SHOT;

	ri->ifindex = ifindex;
	ri->flags = flags;

	return TC_ACT_REDIRECT;
}