static struct sk_buff *
sch_handle_egress(struct sk_buff *skb, int *ret, struct net_device *dev)
{
	struct mini_Qdisc *miniq = rcu_dereference_bh(dev->miniq_egress);
	struct tcf_result cl_res;

	if (!miniq)
		return skb;

	/* qdisc_skb_cb(skb)->pkt_len was already set by the caller. */
	mini_qdisc_bstats_cpu_update(miniq, skb);

	switch (tcf_classify(skb, miniq->filter_list, &cl_res, false)) {
	case TC_ACT_OK:
	case TC_ACT_RECLASSIFY:
		skb->tc_index = TC_H_MIN(cl_res.classid);
		break;
	case TC_ACT_SHOT:
		mini_qdisc_qstats_cpu_drop(miniq);
		*ret = NET_XMIT_DROP;
		kfree_skb(skb);
		return NULL;
	case TC_ACT_STOLEN:
	case TC_ACT_QUEUED:
	case TC_ACT_TRAP:
		*ret = NET_XMIT_SUCCESS;
		consume_skb(skb);
		return NULL;
	case TC_ACT_REDIRECT:
		/* No need to push/pop skb's mac_header here on egress! */
		skb_do_redirect(skb);
		*ret = NET_XMIT_SUCCESS;
		return NULL;
	default:
		break;
	}

	return skb;