static inline int xfrm_policy_check(struct sock *sk, int dir, struct sk_buff *skb, unsigned short family)
{
	return __xfrm_policy_check2(sk, dir, skb, family, 0);
}