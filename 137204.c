bool bpf_helper_changes_pkt_data(void *func)
{
	if (func == bpf_skb_vlan_push ||
	    func == bpf_skb_vlan_pop ||
	    func == bpf_skb_store_bytes ||
	    func == bpf_skb_change_proto ||
	    func == bpf_skb_change_head ||
	    func == bpf_skb_change_tail ||
	    func == bpf_skb_adjust_room ||
	    func == bpf_skb_pull_data ||
	    func == bpf_clone_redirect ||
	    func == bpf_l3_csum_replace ||
	    func == bpf_l4_csum_replace ||
	    func == bpf_xdp_adjust_head ||
	    func == bpf_xdp_adjust_meta ||
	    func == bpf_msg_pull_data)
		return true;

	return false;
}