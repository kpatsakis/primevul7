
void netif_schedule_queue(struct netdev_queue *txq)
{
	rcu_read_lock();
	if (!(txq->state & QUEUE_STATE_ANY_XOFF)) {
		struct Qdisc *q = rcu_dereference(txq->qdisc);

		__netif_schedule(q);
	}
	rcu_read_unlock();