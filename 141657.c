void __xfrm_state_destroy(struct xfrm_state *x, bool sync)
{
	WARN_ON(x->km.state != XFRM_STATE_DEAD);

	if (sync) {
		synchronize_rcu();
		___xfrm_state_destroy(x);
	} else {
		spin_lock_bh(&xfrm_state_gc_lock);
		hlist_add_head(&x->gclist, &xfrm_state_gc_list);
		spin_unlock_bh(&xfrm_state_gc_lock);
		schedule_work(&xfrm_state_gc_work);
	}
}