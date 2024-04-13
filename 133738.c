/* XXX: check that highmem exists at all on the given machine. */
static int illegal_highdma(struct net_device *dev, struct sk_buff *skb)
{
#ifdef CONFIG_HIGHMEM
	int i;

	if (!(dev->features & NETIF_F_HIGHDMA)) {
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			if (PageHighMem(skb_frag_page(frag)))
				return 1;
		}
	}
#endif
	return 0;