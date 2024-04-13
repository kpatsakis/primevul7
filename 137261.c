struct sock *do_sk_redirect_map(struct sk_buff *skb)
{
	struct tcp_skb_cb *tcb = TCP_SKB_CB(skb);
	struct sock *sk = NULL;

	if (tcb->bpf.map) {
		sk = __sock_map_lookup_elem(tcb->bpf.map, tcb->bpf.key);

		tcb->bpf.key = 0;
		tcb->bpf.map = NULL;
	}

	return sk;
}