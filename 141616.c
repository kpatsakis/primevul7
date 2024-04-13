static inline int xfrm_tunnel_check(struct sk_buff *skb, struct xfrm_state *x,
				    unsigned int family)
{
	bool tunnel = false;

	switch(family) {
	case AF_INET:
		if (XFRM_TUNNEL_SKB_CB(skb)->tunnel.ip4)
			tunnel = true;
		break;
	case AF_INET6:
		if (XFRM_TUNNEL_SKB_CB(skb)->tunnel.ip6)
			tunnel = true;
		break;
	}
	if (tunnel && !(x->outer_mode->flags & XFRM_MODE_FLAG_TUNNEL))
		return -EINVAL;

	return 0;
}