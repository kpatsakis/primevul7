/* Network device is going away, flush any packets still pending */
static void flush_backlog(struct work_struct *work)
{
	struct sk_buff *skb, *tmp;
	struct softnet_data *sd;

	local_bh_disable();
	sd = this_cpu_ptr(&softnet_data);

	local_irq_disable();
	rps_lock(sd);
	skb_queue_walk_safe(&sd->input_pkt_queue, skb, tmp) {
		if (skb->dev->reg_state == NETREG_UNREGISTERING) {
			__skb_unlink(skb, &sd->input_pkt_queue);
			kfree_skb(skb);
			input_queue_head_incr(sd);
		}
	}
	rps_unlock(sd);
	local_irq_enable();

	skb_queue_walk_safe(&sd->process_queue, skb, tmp) {
		if (skb->dev->reg_state == NETREG_UNREGISTERING) {
			__skb_unlink(skb, &sd->process_queue);
			kfree_skb(skb);
			input_queue_head_incr(sd);
		}
	}
	local_bh_enable();