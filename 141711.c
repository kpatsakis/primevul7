static inline int __xfrm_policy_check2(struct sock *sk, int dir,
				       struct sk_buff *skb,
				       unsigned int family, int reverse)
{
	struct net *net = dev_net(skb->dev);
	int ndir = dir | (reverse ? XFRM_POLICY_MASK + 1 : 0);

	if (sk && sk->sk_policy[XFRM_POLICY_IN])
		return __xfrm_policy_check(sk, ndir, skb, family);

	return	(!net->xfrm.policy_count[dir] && !secpath_exists(skb)) ||
		(skb_dst(skb)->flags & DST_NOPOLICY) ||
		__xfrm_policy_check(sk, ndir, skb, family);
}