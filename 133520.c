
gro_result_t napi_gro_frags(struct napi_struct *napi)
{
	gro_result_t ret;
	struct sk_buff *skb = napi_frags_skb(napi);

	if (!skb)
		return GRO_DROP;

	trace_napi_gro_frags_entry(skb);

	ret = napi_frags_finish(napi, skb, dev_gro_receive(napi, skb));
	trace_napi_gro_frags_exit(ret);

	return ret;