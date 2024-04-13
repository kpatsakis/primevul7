BPF_CALL_2(bpf_xdp_adjust_meta, struct xdp_buff *, xdp, int, offset)
{
	void *meta = xdp->data_meta + offset;
	unsigned long metalen = xdp->data - meta;

	if (xdp_data_meta_unsupported(xdp))
		return -ENOTSUPP;
	if (unlikely(meta < xdp->data_hard_start ||
		     meta > xdp->data))
		return -EINVAL;
	if (unlikely((metalen & (sizeof(__u32) - 1)) ||
		     (metalen > 32)))
		return -EACCES;

	xdp->data_meta = meta;

	return 0;
}