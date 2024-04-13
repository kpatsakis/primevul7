BPF_CALL_3(bpf_skb_vlan_push, struct sk_buff *, skb, __be16, vlan_proto,
	   u16, vlan_tci)
{
	int ret;

	if (unlikely(vlan_proto != htons(ETH_P_8021Q) &&
		     vlan_proto != htons(ETH_P_8021AD)))
		vlan_proto = htons(ETH_P_8021Q);

	bpf_push_mac_rcsum(skb);
	ret = skb_vlan_push(skb, vlan_proto, vlan_tci);
	bpf_pull_mac_rcsum(skb);

	bpf_compute_data_pointers(skb);
	return ret;
}