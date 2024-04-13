static inline int xfrm_decode_session_reverse(struct sk_buff *skb,
					      struct flowi *fl,
					      unsigned int family)
{
	return __xfrm_decode_session(skb, fl, family, 1);
}