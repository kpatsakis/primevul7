static struct ip_esp_hdr *esp6_output_tcp_encap(struct xfrm_state *x,
						struct sk_buff *skb,
						struct esp_info *esp)
{
	return ERR_PTR(-EOPNOTSUPP);
}