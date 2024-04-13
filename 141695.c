static inline struct xfrm_state *xfrm_input_state(struct sk_buff *skb)
{
	struct sec_path *sp = skb_sec_path(skb);

	return sp->xvec[sp->len - 1];
}