static inline int xfrm_decode_session(struct sk_buff *skb, struct flowi *fl,
				      unsigned int family)
{
	return __xfrm_decode_session(skb, fl, family, 0);
}