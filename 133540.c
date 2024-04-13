
int dev_queue_xmit_accel(struct sk_buff *skb, struct net_device *sb_dev)
{
	return __dev_queue_xmit(skb, sb_dev);