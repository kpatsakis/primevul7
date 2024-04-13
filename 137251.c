static int __xdp_generic_ok_fwd_dev(struct sk_buff *skb, struct net_device *fwd)
{
	unsigned int len;

	if (unlikely(!(fwd->flags & IFF_UP)))
		return -ENETDOWN;

	len = fwd->mtu + fwd->hard_header_len + VLAN_HLEN;
	if (skb->len > len)
		return -EMSGSIZE;

	return 0;
}