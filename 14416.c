static struct ip_esp_hdr *esp6_output_tcp_encap(struct xfrm_state *x,
						struct sk_buff *skb,
						struct esp_info *esp)
{
	__be16 *lenp = (void *)esp->esph;
	struct ip_esp_hdr *esph;
	unsigned int len;
	struct sock *sk;

	len = skb->len + esp->tailen - skb_transport_offset(skb);
	if (len > IP_MAX_MTU)
		return ERR_PTR(-EMSGSIZE);

	rcu_read_lock();
	sk = esp6_find_tcp_sk(x);
	rcu_read_unlock();

	if (IS_ERR(sk))
		return ERR_CAST(sk);

	*lenp = htons(len);
	esph = (struct ip_esp_hdr *)(lenp + 1);

	return esph;
}