static int copy_to_user_kmaddress(const struct xfrm_kmaddress *k, struct sk_buff *skb)
{
	struct xfrm_user_kmaddress uk;

	memset(&uk, 0, sizeof(uk));
	uk.family = k->family;
	uk.reserved = k->reserved;
	memcpy(&uk.local, &k->local, sizeof(uk.local));
	memcpy(&uk.remote, &k->remote, sizeof(uk.remote));

	return nla_put(skb, XFRMA_KMADDRESS, sizeof(uk), &uk);
}