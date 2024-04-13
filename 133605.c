
int netif_rx(struct sk_buff *skb)
{
	int ret;

	trace_netif_rx_entry(skb);

	ret = netif_rx_internal(skb);
	trace_netif_rx_exit(ret);

	return ret;