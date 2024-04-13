static inline int xfrm4_policy_check_reverse(struct sock *sk, int dir,
					     struct sk_buff *skb)
{
	return __xfrm_policy_check2(sk, dir, skb, AF_INET, 1);
}