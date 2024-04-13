BPF_CALL_5(bpf_l4_csum_replace, struct sk_buff *, skb, u32, offset,
	   u64, from, u64, to, u64, flags)
{
	bool is_pseudo = flags & BPF_F_PSEUDO_HDR;
	bool is_mmzero = flags & BPF_F_MARK_MANGLED_0;
	bool do_mforce = flags & BPF_F_MARK_ENFORCE;
	__sum16 *ptr;

	if (unlikely(flags & ~(BPF_F_MARK_MANGLED_0 | BPF_F_MARK_ENFORCE |
			       BPF_F_PSEUDO_HDR | BPF_F_HDR_FIELD_MASK)))
		return -EINVAL;
	if (unlikely(offset > 0xffff || offset & 1))
		return -EFAULT;
	if (unlikely(bpf_try_make_writable(skb, offset + sizeof(*ptr))))
		return -EFAULT;

	ptr = (__sum16 *)(skb->data + offset);
	if (is_mmzero && !do_mforce && !*ptr)
		return 0;

	switch (flags & BPF_F_HDR_FIELD_MASK) {
	case 0:
		if (unlikely(from != 0))
			return -EINVAL;

		inet_proto_csum_replace_by_diff(ptr, skb, to, is_pseudo);
		break;
	case 2:
		inet_proto_csum_replace2(ptr, skb, from, to, is_pseudo);
		break;
	case 4:
		inet_proto_csum_replace4(ptr, skb, from, to, is_pseudo);
		break;
	default:
		return -EINVAL;
	}

	if (is_mmzero && !*ptr)
		*ptr = CSUM_MANGLED_0;
	return 0;
}