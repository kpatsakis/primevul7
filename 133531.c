
u16 dev_pick_tx_cpu_id(struct net_device *dev, struct sk_buff *skb,
		       struct net_device *sb_dev)
{
	return (u16)raw_smp_processor_id() % dev->real_num_tx_queues;