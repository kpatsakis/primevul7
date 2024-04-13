static inline int xfrm_route_forward(struct sk_buff *skb, unsigned short family)
{
	struct net *net = dev_net(skb->dev);

	return	!net->xfrm.policy_count[XFRM_POLICY_OUT] ||
		(skb_dst(skb)->flags & DST_NOXFRM) ||
		__xfrm_route_forward(skb, family);
}