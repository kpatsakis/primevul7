BPF_CALL_4(bpf_skb_set_tunnel_key, struct sk_buff *, skb,
	   const struct bpf_tunnel_key *, from, u32, size, u64, flags)
{
	struct metadata_dst *md = this_cpu_ptr(md_dst);
	u8 compat[sizeof(struct bpf_tunnel_key)];
	struct ip_tunnel_info *info;

	if (unlikely(flags & ~(BPF_F_TUNINFO_IPV6 | BPF_F_ZERO_CSUM_TX |
			       BPF_F_DONT_FRAGMENT | BPF_F_SEQ_NUMBER)))
		return -EINVAL;
	if (unlikely(size != sizeof(struct bpf_tunnel_key))) {
		switch (size) {
		case offsetof(struct bpf_tunnel_key, tunnel_label):
		case offsetof(struct bpf_tunnel_key, tunnel_ext):
		case offsetof(struct bpf_tunnel_key, remote_ipv6[1]):
			/* Fixup deprecated structure layouts here, so we have
			 * a common path later on.
			 */
			memcpy(compat, from, size);
			memset(compat + size, 0, sizeof(compat) - size);
			from = (const struct bpf_tunnel_key *) compat;
			break;
		default:
			return -EINVAL;
		}
	}
	if (unlikely((!(flags & BPF_F_TUNINFO_IPV6) && from->tunnel_label) ||
		     from->tunnel_ext))
		return -EINVAL;

	skb_dst_drop(skb);
	dst_hold((struct dst_entry *) md);
	skb_dst_set(skb, (struct dst_entry *) md);

	info = &md->u.tun_info;
	memset(info, 0, sizeof(*info));
	info->mode = IP_TUNNEL_INFO_TX;

	info->key.tun_flags = TUNNEL_KEY | TUNNEL_CSUM | TUNNEL_NOCACHE;
	if (flags & BPF_F_DONT_FRAGMENT)
		info->key.tun_flags |= TUNNEL_DONT_FRAGMENT;
	if (flags & BPF_F_ZERO_CSUM_TX)
		info->key.tun_flags &= ~TUNNEL_CSUM;
	if (flags & BPF_F_SEQ_NUMBER)
		info->key.tun_flags |= TUNNEL_SEQ;

	info->key.tun_id = cpu_to_be64(from->tunnel_id);
	info->key.tos = from->tunnel_tos;
	info->key.ttl = from->tunnel_ttl;

	if (flags & BPF_F_TUNINFO_IPV6) {
		info->mode |= IP_TUNNEL_INFO_IPV6;
		memcpy(&info->key.u.ipv6.dst, from->remote_ipv6,
		       sizeof(from->remote_ipv6));
		info->key.label = cpu_to_be32(from->tunnel_label) &
				  IPV6_FLOWLABEL_MASK;
	} else {
		info->key.u.ipv4.dst = cpu_to_be32(from->remote_ipv4);
	}

	return 0;
}