static inline int xfrm4_udp_encap_rcv(struct sock *sk, struct sk_buff *skb)
{
 	/* should not happen */
 	kfree_skb(skb);
	return 0;
}