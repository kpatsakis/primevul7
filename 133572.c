
int netif_rx_ni(struct sk_buff *skb)
{
	int err;

	trace_netif_rx_ni_entry(skb);

	preempt_disable();
	err = netif_rx_internal(skb);
	if (local_softirq_pending())
		do_softirq();
	preempt_enable();
	trace_netif_rx_ni_exit(err);

	return err;