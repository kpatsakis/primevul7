static void esp_output_encap_csum(struct sk_buff *skb)
{
	/* UDP encap with IPv6 requires a valid checksum */
	if (*skb_mac_header(skb) == IPPROTO_UDP) {
		struct udphdr *uh = udp_hdr(skb);
		struct ipv6hdr *ip6h = ipv6_hdr(skb);
		int len = ntohs(uh->len);
		unsigned int offset = skb_transport_offset(skb);
		__wsum csum = skb_checksum(skb, offset, skb->len - offset, 0);

		uh->check = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr,
					    len, IPPROTO_UDP, csum);
		if (uh->check == 0)
			uh->check = CSUM_MANGLED_0;
	}
}