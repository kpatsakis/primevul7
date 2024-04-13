bpf_get_skb_set_tunnel_proto(enum bpf_func_id which)
{
	if (!md_dst) {
		struct metadata_dst __percpu *tmp;

		tmp = metadata_dst_alloc_percpu(IP_TUNNEL_OPTS_MAX,
						METADATA_IP_TUNNEL,
						GFP_KERNEL);
		if (!tmp)
			return NULL;
		if (cmpxchg(&md_dst, NULL, tmp))
			metadata_dst_free_percpu(tmp);
	}

	switch (which) {
	case BPF_FUNC_skb_set_tunnel_key:
		return &bpf_skb_set_tunnel_key_proto;
	case BPF_FUNC_skb_set_tunnel_opt:
		return &bpf_skb_set_tunnel_opt_proto;
	default:
		return NULL;
	}
}