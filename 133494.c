
static netdev_features_t harmonize_features(struct sk_buff *skb,
	netdev_features_t features)
{
	int tmp;
	__be16 type;

	type = skb_network_protocol(skb, &tmp);
	features = net_mpls_features(skb, features, type);

	if (skb->ip_summed != CHECKSUM_NONE &&
	    !can_checksum_protocol(features, type)) {
		features &= ~(NETIF_F_CSUM_MASK | NETIF_F_GSO_MASK);
	}
	if (illegal_highdma(skb->dev, skb))
		features &= ~NETIF_F_SG;

	return features;