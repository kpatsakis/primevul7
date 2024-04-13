BPF_CALL_3(bpf_skb_get_tunnel_opt, struct sk_buff *, skb, u8 *, to, u32, size)
{
	const struct ip_tunnel_info *info = skb_tunnel_info(skb);
	int err;

	if (unlikely(!info ||
		     !(info->key.tun_flags & TUNNEL_OPTIONS_PRESENT))) {
		err = -ENOENT;
		goto err_clear;
	}
	if (unlikely(size < info->options_len)) {
		err = -ENOMEM;
		goto err_clear;
	}

	ip_tunnel_info_opts_get(to, info);
	if (size > info->options_len)
		memset(to + info->options_len, 0, size - info->options_len);

	return info->options_len;
err_clear:
	memset(to, 0, size);
	return err;
}