static int xfrm6_tunnel_input(struct xfrm_state *x, struct sk_buff *skb)
{
	return skb_network_header(skb)[IP6CB(skb)->nhoff];
}