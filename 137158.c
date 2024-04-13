BPF_CALL_1(bpf_get_route_realm, const struct sk_buff *, skb)
{
	return dst_tclassid(skb);
}