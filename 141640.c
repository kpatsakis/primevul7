secpath_reset(struct sk_buff *skb)
{
#ifdef CONFIG_XFRM
	skb_ext_del(skb, SKB_EXT_SEC_PATH);
#endif
}