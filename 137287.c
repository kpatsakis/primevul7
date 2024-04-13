BPF_CALL_5(bpf_xdp_event_output, struct xdp_buff *, xdp, struct bpf_map *, map,
	   u64, flags, void *, meta, u64, meta_size)
{
	u64 xdp_size = (flags & BPF_F_CTXLEN_MASK) >> 32;

	if (unlikely(flags & ~(BPF_F_CTXLEN_MASK | BPF_F_INDEX_MASK)))
		return -EINVAL;
	if (unlikely(xdp_size > (unsigned long)(xdp->data_end - xdp->data)))
		return -EFAULT;

	return bpf_event_output(map, flags, meta, meta_size, xdp->data,
				xdp_size, bpf_xdp_copy);
}