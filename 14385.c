static struct ip_esp_hdr *esp6_output_udp_encap(struct sk_buff *skb,
					       int encap_type,
					       struct esp_info *esp,
					       __be16 sport,
					       __be16 dport)
{
	struct udphdr *uh;
	__be32 *udpdata32;
	unsigned int len;

	len = skb->len + esp->tailen - skb_transport_offset(skb);
	if (len > U16_MAX)
		return ERR_PTR(-EMSGSIZE);

	uh = (struct udphdr *)esp->esph;
	uh->source = sport;
	uh->dest = dport;
	uh->len = htons(len);
	uh->check = 0;

	*skb_mac_header(skb) = IPPROTO_UDP;

	if (encap_type == UDP_ENCAP_ESPINUDP_NON_IKE) {
		udpdata32 = (__be32 *)(uh + 1);
		udpdata32[0] = udpdata32[1] = 0;
		return (struct ip_esp_hdr *)(udpdata32 + 2);
	}

	return (struct ip_esp_hdr *)(uh + 1);
}