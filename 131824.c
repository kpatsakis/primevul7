ieee80211_rx_h_mesh_fwding(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *fwd_hdr, *hdr;
	struct ieee80211_tx_info *info;
	struct ieee80211s_hdr *mesh_hdr;
	struct sk_buff *skb = rx->skb, *fwd_skb;
	struct ieee80211_local *local = rx->local;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u16 ac, q, hdrlen;
	int tailroom = 0;

	hdr = (struct ieee80211_hdr *) skb->data;
	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	/* make sure fixed part of mesh header is there, also checks skb len */
	if (!pskb_may_pull(rx->skb, hdrlen + 6))
		return RX_DROP_MONITOR;

	mesh_hdr = (struct ieee80211s_hdr *) (skb->data + hdrlen);

	/* make sure full mesh header is there, also checks skb len */
	if (!pskb_may_pull(rx->skb,
			   hdrlen + ieee80211_get_mesh_hdrlen(mesh_hdr)))
		return RX_DROP_MONITOR;

	/* reload pointers */
	hdr = (struct ieee80211_hdr *) skb->data;
	mesh_hdr = (struct ieee80211s_hdr *) (skb->data + hdrlen);

	if (ieee80211_drop_unencrypted(rx, hdr->frame_control))
		return RX_DROP_MONITOR;

	/* frame is in RMC, don't forward */
	if (ieee80211_is_data(hdr->frame_control) &&
	    is_multicast_ether_addr(hdr->addr1) &&
	    mesh_rmc_check(rx->sdata, hdr->addr3, mesh_hdr))
		return RX_DROP_MONITOR;

	if (!ieee80211_is_data(hdr->frame_control))
		return RX_CONTINUE;

	if (!mesh_hdr->ttl)
		return RX_DROP_MONITOR;

	if (mesh_hdr->flags & MESH_FLAGS_AE) {
		struct mesh_path *mppath;
		char *proxied_addr;
		char *mpp_addr;

		if (is_multicast_ether_addr(hdr->addr1)) {
			mpp_addr = hdr->addr3;
			proxied_addr = mesh_hdr->eaddr1;
		} else if ((mesh_hdr->flags & MESH_FLAGS_AE) ==
			    MESH_FLAGS_AE_A5_A6) {
			/* has_a4 already checked in ieee80211_rx_mesh_check */
			mpp_addr = hdr->addr4;
			proxied_addr = mesh_hdr->eaddr2;
		} else {
			return RX_DROP_MONITOR;
		}

		rcu_read_lock();
		mppath = mpp_path_lookup(sdata, proxied_addr);
		if (!mppath) {
			mpp_path_add(sdata, proxied_addr, mpp_addr);
		} else {
			spin_lock_bh(&mppath->state_lock);
			if (!ether_addr_equal(mppath->mpp, mpp_addr))
				memcpy(mppath->mpp, mpp_addr, ETH_ALEN);
			mppath->exp_time = jiffies;
			spin_unlock_bh(&mppath->state_lock);
		}
		rcu_read_unlock();
	}

	/* Frame has reached destination.  Don't forward */
	if (!is_multicast_ether_addr(hdr->addr1) &&
	    ether_addr_equal(sdata->vif.addr, hdr->addr3))
		return RX_CONTINUE;

	ac = ieee80211_select_queue_80211(sdata, skb, hdr);
	q = sdata->vif.hw_queue[ac];
	if (ieee80211_queue_stopped(&local->hw, q)) {
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, dropped_frames_congestion);
		return RX_DROP_MONITOR;
	}
	skb_set_queue_mapping(skb, q);

	if (!--mesh_hdr->ttl) {
		if (!is_multicast_ether_addr(hdr->addr1))
			IEEE80211_IFSTA_MESH_CTR_INC(ifmsh,
						     dropped_frames_ttl);
		goto out;
	}

	if (!ifmsh->mshcfg.dot11MeshForwarding)
		goto out;

	if (sdata->crypto_tx_tailroom_needed_cnt)
		tailroom = IEEE80211_ENCRYPT_TAILROOM;

	fwd_skb = skb_copy_expand(skb, local->tx_headroom +
				       sdata->encrypt_headroom,
				  tailroom, GFP_ATOMIC);
	if (!fwd_skb)
		goto out;

	fwd_hdr =  (struct ieee80211_hdr *) fwd_skb->data;
	fwd_hdr->frame_control &= ~cpu_to_le16(IEEE80211_FCTL_RETRY);
	info = IEEE80211_SKB_CB(fwd_skb);
	memset(info, 0, sizeof(*info));
	info->flags |= IEEE80211_TX_INTFL_NEED_TXPROCESSING;
	info->control.vif = &rx->sdata->vif;
	info->control.jiffies = jiffies;
	if (is_multicast_ether_addr(fwd_hdr->addr1)) {
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_mcast);
		memcpy(fwd_hdr->addr2, sdata->vif.addr, ETH_ALEN);
		/* update power mode indication when forwarding */
		ieee80211_mps_set_frame_flags(sdata, NULL, fwd_hdr);
	} else if (!mesh_nexthop_lookup(sdata, fwd_skb)) {
		/* mesh power mode flags updated in mesh_nexthop_lookup */
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_unicast);
	} else {
		/* unable to resolve next hop */
		mesh_path_error_tx(sdata, ifmsh->mshcfg.element_ttl,
				   fwd_hdr->addr3, 0,
				   WLAN_REASON_MESH_PATH_NOFORWARD,
				   fwd_hdr->addr2);
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, dropped_frames_no_route);
		kfree_skb(fwd_skb);
		return RX_DROP_MONITOR;
	}

	IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_frames);
	ieee80211_add_pending_skb(local, fwd_skb);
 out:
	if (is_multicast_ether_addr(hdr->addr1))
		return RX_CONTINUE;
	return RX_DROP_MONITOR;
}