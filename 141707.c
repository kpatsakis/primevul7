static void xfrm_state_gc_task(struct work_struct *work)
{
	struct xfrm_state *x;
	struct hlist_node *tmp;
	struct hlist_head gc_list;

	spin_lock_bh(&xfrm_state_gc_lock);
	hlist_move_list(&xfrm_state_gc_list, &gc_list);
	spin_unlock_bh(&xfrm_state_gc_lock);

	synchronize_rcu();

	hlist_for_each_entry_safe(x, tmp, &gc_list, gclist)
		___xfrm_state_destroy(x);
}