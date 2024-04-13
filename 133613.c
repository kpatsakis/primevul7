
static struct list_head *gro_list_prepare(struct napi_struct *napi,
					  struct sk_buff *skb)
{
	unsigned int maclen = skb->dev->hard_header_len;
	u32 hash = skb_get_hash_raw(skb);
	struct list_head *head;
	struct sk_buff *p;

	head = &napi->gro_hash[hash & (GRO_HASH_BUCKETS - 1)].list;
	list_for_each_entry(p, head, list) {
		unsigned long diffs;

		NAPI_GRO_CB(p)->flush = 0;

		if (hash != skb_get_hash_raw(p)) {
			NAPI_GRO_CB(p)->same_flow = 0;
			continue;
		}

		diffs = (unsigned long)p->dev ^ (unsigned long)skb->dev;
		diffs |= skb_vlan_tag_present(p) ^ skb_vlan_tag_present(skb);
		if (skb_vlan_tag_present(p))
			diffs |= p->vlan_tci ^ skb->vlan_tci;
		diffs |= skb_metadata_dst_cmp(p, skb);
		diffs |= skb_metadata_differs(p, skb);
		if (maclen == ETH_HLEN)
			diffs |= compare_ether_header(skb_mac_header(p),
						      skb_mac_header(skb));
		else if (!diffs)
			diffs = memcmp(skb_mac_header(p),
				       skb_mac_header(skb),
				       maclen);
		NAPI_GRO_CB(p)->same_flow = !diffs;
	}

	return head;