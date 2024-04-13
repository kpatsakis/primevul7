							   struct sk_buff *));
static enum gro_result dev_gro_receive(struct napi_struct *napi, struct sk_buff *skb)
{
	u32 hash = skb_get_hash_raw(skb) & (GRO_HASH_BUCKETS - 1);
	struct list_head *head = &offload_base;
	struct packet_offload *ptype;
	__be16 type = skb->protocol;
	struct list_head *gro_head;
	struct sk_buff *pp = NULL;
	enum gro_result ret;
	int same_flow;
	int grow;

	if (netif_elide_gro(skb->dev))
		goto normal;

	gro_head = gro_list_prepare(napi, skb);

	rcu_read_lock();
	list_for_each_entry_rcu(ptype, head, list) {
		if (ptype->type != type || !ptype->callbacks.gro_receive)
			continue;

		skb_set_network_header(skb, skb_gro_offset(skb));
		skb_reset_mac_len(skb);
		NAPI_GRO_CB(skb)->same_flow = 0;
		NAPI_GRO_CB(skb)->flush = skb_is_gso(skb) || skb_has_frag_list(skb);
		NAPI_GRO_CB(skb)->free = 0;
		NAPI_GRO_CB(skb)->encap_mark = 0;
		NAPI_GRO_CB(skb)->recursion_counter = 0;
		NAPI_GRO_CB(skb)->is_fou = 0;
		NAPI_GRO_CB(skb)->is_atomic = 1;
		NAPI_GRO_CB(skb)->gro_remcsum_start = 0;

		/* Setup for GRO checksum validation */
		switch (skb->ip_summed) {
		case CHECKSUM_COMPLETE:
			NAPI_GRO_CB(skb)->csum = skb->csum;
			NAPI_GRO_CB(skb)->csum_valid = 1;
			NAPI_GRO_CB(skb)->csum_cnt = 0;
			break;
		case CHECKSUM_UNNECESSARY:
			NAPI_GRO_CB(skb)->csum_cnt = skb->csum_level + 1;
			NAPI_GRO_CB(skb)->csum_valid = 0;
			break;
		default:
			NAPI_GRO_CB(skb)->csum_cnt = 0;
			NAPI_GRO_CB(skb)->csum_valid = 0;
		}

		pp = INDIRECT_CALL_INET(ptype->callbacks.gro_receive,
					ipv6_gro_receive, inet_gro_receive,
					gro_head, skb);
		break;
	}
	rcu_read_unlock();

	if (&ptype->list == head)
		goto normal;

	if (IS_ERR(pp) && PTR_ERR(pp) == -EINPROGRESS) {
		ret = GRO_CONSUMED;
		goto ok;
	}

	same_flow = NAPI_GRO_CB(skb)->same_flow;
	ret = NAPI_GRO_CB(skb)->free ? GRO_MERGED_FREE : GRO_MERGED;

	if (pp) {
		skb_list_del_init(pp);
		napi_gro_complete(pp);
		napi->gro_hash[hash].count--;
	}

	if (same_flow)
		goto ok;

	if (NAPI_GRO_CB(skb)->flush)
		goto normal;

	if (unlikely(napi->gro_hash[hash].count >= MAX_GRO_SKBS)) {
		gro_flush_oldest(gro_head);
	} else {
		napi->gro_hash[hash].count++;
	}
	NAPI_GRO_CB(skb)->count = 1;
	NAPI_GRO_CB(skb)->age = jiffies;
	NAPI_GRO_CB(skb)->last = skb;
	skb_shinfo(skb)->gso_size = skb_gro_len(skb);
	list_add(&skb->list, gro_head);
	ret = GRO_HELD;

pull:
	grow = skb_gro_offset(skb) - skb_headlen(skb);
	if (grow > 0)
		gro_pull_from_frag0(skb, grow);
ok:
	if (napi->gro_hash[hash].count) {
		if (!test_bit(hash, &napi->gro_bitmask))
			__set_bit(hash, &napi->gro_bitmask);
	} else if (test_bit(hash, &napi->gro_bitmask)) {
		__clear_bit(hash, &napi->gro_bitmask);
	}

	return ret;

normal:
	ret = GRO_NORMAL;
	goto pull;