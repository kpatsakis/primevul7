BPF_CALL_1(bpf_get_socket_cookie, struct sk_buff *, skb)
{
	return skb->sk ? sock_gen_cookie(skb->sk) : 0;
}