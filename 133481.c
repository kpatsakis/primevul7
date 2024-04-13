
struct sk_buff *napi_get_frags(struct napi_struct *napi)
{
	struct sk_buff *skb = napi->skb;

	if (!skb) {
		skb = napi_alloc_skb(napi, GRO_MAX_HEAD);
		if (skb) {
			napi->skb = skb;
			skb_mark_napi_id(skb, napi);
		}
	}
	return skb;