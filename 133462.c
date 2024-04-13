
static void napi_skb_free_stolen_head(struct sk_buff *skb)
{
	skb_dst_drop(skb);
	secpath_reset(skb);
	kmem_cache_free(skbuff_head_cache, skb);