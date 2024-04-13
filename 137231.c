BPF_CALL_4(bpf_skb_get_tunnel_key, struct sk_buff *, skb, struct bpf_tunnel_key *, to,
	   u32, size, u64, flags)
{
	const struct ip_tunnel_info *info = skb_tunnel_info(skb);
	u8 compat[sizeof(struct bpf_tunnel_key)];
	void *to_orig = to;
	int err;

	if (unlikely(!info || (flags & ~(BPF_F_TUNINFO_IPV6)))) {
		err = -EINVAL;
		goto err_clear;
	}
	if (ip_tunnel_info_af(info) != bpf_tunnel_key_af(flags)) {
		err = -EPROTO;
		goto err_clear;
	}
	if (unlikely(size != sizeof(struct bpf_tunnel_key))) {
		err = -EINVAL;
		switch (size) {
		case offsetof(struct bpf_tunnel_key, tunnel_label):
		case offsetof(struct bpf_tunnel_key, tunnel_ext):
			goto set_compat;
		case offsetof(struct bpf_tunnel_key, remote_ipv6[1]):
			/* Fixup deprecated structure layouts here, so we have
			 * a common path later on.
			 */
			if (ip_tunnel_info_af(info) != AF_INET)
				goto err_clear;
set_compat:
			to = (struct bpf_tunnel_key *)compat;
			break;
		default:
			goto err_clear;
		}
	}

	to->tunnel_id = be64_to_cpu(info->key.tun_id);
	to->tunnel_tos = info->key.tos;
	to->tunnel_ttl = info->key.ttl;

	if (flags & BPF_F_TUNINFO_IPV6) {
		memcpy(to->remote_ipv6, &info->key.u.ipv6.src,
		       sizeof(to->remote_ipv6));
		to->tunnel_label = be32_to_cpu(info->key.label);
	} else {
		to->remote_ipv4 = be32_to_cpu(info->key.u.ipv4.src);
	}

	if (unlikely(size != sizeof(struct bpf_tunnel_key)))
		memcpy(to_orig, to, size);

	return 0;
err_clear:
	memset(to_orig, 0, size);
	return err;
}