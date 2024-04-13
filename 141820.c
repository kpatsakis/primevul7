xfrm_state_find(const xfrm_address_t *daddr, const xfrm_address_t *saddr,
		const struct flowi *fl, struct xfrm_tmpl *tmpl,
		struct xfrm_policy *pol, int *err,
		unsigned short family, u32 if_id)
{
	static xfrm_address_t saddr_wildcard = { };
	struct net *net = xp_net(pol);
	unsigned int h, h_wildcard;
	struct xfrm_state *x, *x0, *to_put;
	int acquire_in_progress = 0;
	int error = 0;
	struct xfrm_state *best = NULL;
	u32 mark = pol->mark.v & pol->mark.m;
	unsigned short encap_family = tmpl->encap_family;
	unsigned int sequence;
	struct km_event c;

	to_put = NULL;

	sequence = read_seqcount_begin(&xfrm_state_hash_generation);

	rcu_read_lock();
	h = xfrm_dst_hash(net, daddr, saddr, tmpl->reqid, encap_family);
	hlist_for_each_entry_rcu(x, net->xfrm.state_bydst + h, bydst) {
		if (x->props.family == encap_family &&
		    x->props.reqid == tmpl->reqid &&
		    (mark & x->mark.m) == x->mark.v &&
		    x->if_id == if_id &&
		    !(x->props.flags & XFRM_STATE_WILDRECV) &&
		    xfrm_state_addr_check(x, daddr, saddr, encap_family) &&
		    tmpl->mode == x->props.mode &&
		    tmpl->id.proto == x->id.proto &&
		    (tmpl->id.spi == x->id.spi || !tmpl->id.spi))
			xfrm_state_look_at(pol, x, fl, encap_family,
					   &best, &acquire_in_progress, &error);
	}
	if (best || acquire_in_progress)
		goto found;

	h_wildcard = xfrm_dst_hash(net, daddr, &saddr_wildcard, tmpl->reqid, encap_family);
	hlist_for_each_entry_rcu(x, net->xfrm.state_bydst + h_wildcard, bydst) {
		if (x->props.family == encap_family &&
		    x->props.reqid == tmpl->reqid &&
		    (mark & x->mark.m) == x->mark.v &&
		    x->if_id == if_id &&
		    !(x->props.flags & XFRM_STATE_WILDRECV) &&
		    xfrm_addr_equal(&x->id.daddr, daddr, encap_family) &&
		    tmpl->mode == x->props.mode &&
		    tmpl->id.proto == x->id.proto &&
		    (tmpl->id.spi == x->id.spi || !tmpl->id.spi))
			xfrm_state_look_at(pol, x, fl, encap_family,
					   &best, &acquire_in_progress, &error);
	}

found:
	x = best;
	if (!x && !error && !acquire_in_progress) {
		if (tmpl->id.spi &&
		    (x0 = __xfrm_state_lookup(net, mark, daddr, tmpl->id.spi,
					      tmpl->id.proto, encap_family)) != NULL) {
			to_put = x0;
			error = -EEXIST;
			goto out;
		}

		c.net = net;
		/* If the KMs have no listeners (yet...), avoid allocating an SA
		 * for each and every packet - garbage collection might not
		 * handle the flood.
		 */
		if (!km_is_alive(&c)) {
			error = -ESRCH;
			goto out;
		}

		x = xfrm_state_alloc(net);
		if (x == NULL) {
			error = -ENOMEM;
			goto out;
		}
		/* Initialize temporary state matching only
		 * to current session. */
		xfrm_init_tempstate(x, fl, tmpl, daddr, saddr, family);
		memcpy(&x->mark, &pol->mark, sizeof(x->mark));
		x->if_id = if_id;

		error = security_xfrm_state_alloc_acquire(x, pol->security, fl->flowi_secid);
		if (error) {
			x->km.state = XFRM_STATE_DEAD;
			to_put = x;
			x = NULL;
			goto out;
		}

		if (km_query(x, tmpl, pol) == 0) {
			spin_lock_bh(&net->xfrm.xfrm_state_lock);
			x->km.state = XFRM_STATE_ACQ;
			list_add(&x->km.all, &net->xfrm.state_all);
			hlist_add_head_rcu(&x->bydst, net->xfrm.state_bydst + h);
			h = xfrm_src_hash(net, daddr, saddr, encap_family);
			hlist_add_head_rcu(&x->bysrc, net->xfrm.state_bysrc + h);
			if (x->id.spi) {
				h = xfrm_spi_hash(net, &x->id.daddr, x->id.spi, x->id.proto, encap_family);
				hlist_add_head_rcu(&x->byspi, net->xfrm.state_byspi + h);
			}
			x->lft.hard_add_expires_seconds = net->xfrm.sysctl_acq_expires;
			tasklet_hrtimer_start(&x->mtimer, ktime_set(net->xfrm.sysctl_acq_expires, 0), HRTIMER_MODE_REL);
			net->xfrm.state_num++;
			xfrm_hash_grow_check(net, x->bydst.next != NULL);
			spin_unlock_bh(&net->xfrm.xfrm_state_lock);
		} else {
			x->km.state = XFRM_STATE_DEAD;
			to_put = x;
			x = NULL;
			error = -ESRCH;
		}
	}
out:
	if (x) {
		if (!xfrm_state_hold_rcu(x)) {
			*err = -EAGAIN;
			x = NULL;
		}
	} else {
		*err = acquire_in_progress ? -EAGAIN : error;
	}
	rcu_read_unlock();
	if (to_put)
		xfrm_state_put(to_put);

	if (read_seqcount_retry(&xfrm_state_hash_generation, sequence)) {
		*err = -EAGAIN;
		if (x) {
			xfrm_state_put(x);
			x = NULL;
		}
	}

	return x;
}