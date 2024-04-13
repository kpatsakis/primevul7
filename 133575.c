
int skb_csum_hwoffload_help(struct sk_buff *skb,
			    const netdev_features_t features)
{
	if (unlikely(skb->csum_not_inet))
		return !!(features & NETIF_F_SCTP_CRC) ? 0 :
			skb_crc32c_csum_help(skb);

	return !!(features & NETIF_F_CSUM_MASK) ? 0 : skb_checksum_help(skb);