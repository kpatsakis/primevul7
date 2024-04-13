BPF_CALL_1(bpf_skb_vlan_pop, struct sk_buff *, skb)
{
	int ret;

	bpf_push_mac_rcsum(skb);
	ret = skb_vlan_pop(skb);
	bpf_pull_mac_rcsum(skb);

	bpf_compute_data_pointers(skb);
	return ret;
}