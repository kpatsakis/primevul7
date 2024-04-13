BPF_CALL_3(bpf_skb_set_tunnel_opt, struct sk_buff *, skb,
	   const u8 *, from, u32, size)
{
	struct ip_tunnel_info *info = skb_tunnel_info(skb);
	const struct metadata_dst *md = this_cpu_ptr(md_dst);

	if (unlikely(info != &md->u.tun_info || (size & (sizeof(u32) - 1))))
		return -EINVAL;
	if (unlikely(size > IP_TUNNEL_OPTS_MAX))
		return -ENOMEM;

	ip_tunnel_info_opts_set(info, from, size);

	return 0;
}