static int esp_output_tcp_encap_cb(struct net *net, struct sock *sk,
				   struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct xfrm_state *x = dst->xfrm;

	return esp_output_tcp_finish(x, skb);
}