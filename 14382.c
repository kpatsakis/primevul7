static int esp6_output_encap(struct xfrm_state *x, struct sk_buff *skb,
			    struct esp_info *esp)
{
	struct xfrm_encap_tmpl *encap = x->encap;
	struct ip_esp_hdr *esph;
	__be16 sport, dport;
	int encap_type;

	spin_lock_bh(&x->lock);
	sport = encap->encap_sport;
	dport = encap->encap_dport;
	encap_type = encap->encap_type;
	spin_unlock_bh(&x->lock);

	switch (encap_type) {
	default:
	case UDP_ENCAP_ESPINUDP:
	case UDP_ENCAP_ESPINUDP_NON_IKE:
		esph = esp6_output_udp_encap(skb, encap_type, esp, sport, dport);
		break;
	case TCP_ENCAP_ESPINTCP:
		esph = esp6_output_tcp_encap(x, skb, esp);
		break;
	}

	if (IS_ERR(esph))
		return PTR_ERR(esph);

	esp->esph = esph;

	return 0;
}