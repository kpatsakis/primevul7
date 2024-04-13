#ifdef CONFIG_BUG
void netdev_rx_csum_fault(struct net_device *dev, struct sk_buff *skb)
{
	if (net_ratelimit()) {
		pr_err("%s: hw csum failure\n", dev ? dev->name : "<unknown>");
		if (dev)
			pr_err("dev features: %pNF\n", &dev->features);
		pr_err("skb len=%u data_len=%u pkt_type=%u gso_size=%u gso_type=%u nr_frags=%u ip_summed=%u csum=%x csum_complete_sw=%d csum_valid=%d csum_level=%u\n",
		       skb->len, skb->data_len, skb->pkt_type,
		       skb_shinfo(skb)->gso_size, skb_shinfo(skb)->gso_type,
		       skb_shinfo(skb)->nr_frags, skb->ip_summed, skb->csum,
		       skb->csum_complete_sw, skb->csum_valid, skb->csum_level);
		dump_stack();
	}