static void xfrm_hash_resize(struct work_struct *work)
{
	struct net *net = container_of(work, struct net, xfrm.state_hash_work);
	struct hlist_head *ndst, *nsrc, *nspi, *odst, *osrc, *ospi;
	unsigned long nsize, osize;
	unsigned int nhashmask, ohashmask;
	int i;

	nsize = xfrm_hash_new_size(net->xfrm.state_hmask);
	ndst = xfrm_hash_alloc(nsize);
	if (!ndst)
		return;
	nsrc = xfrm_hash_alloc(nsize);
	if (!nsrc) {
		xfrm_hash_free(ndst, nsize);
		return;
	}
	nspi = xfrm_hash_alloc(nsize);
	if (!nspi) {
		xfrm_hash_free(ndst, nsize);
		xfrm_hash_free(nsrc, nsize);
		return;
	}

	spin_lock_bh(&net->xfrm.xfrm_state_lock);
	write_seqcount_begin(&xfrm_state_hash_generation);

	nhashmask = (nsize / sizeof(struct hlist_head)) - 1U;
	odst = xfrm_state_deref_prot(net->xfrm.state_bydst, net);
	for (i = net->xfrm.state_hmask; i >= 0; i--)
		xfrm_hash_transfer(odst + i, ndst, nsrc, nspi, nhashmask);

	osrc = xfrm_state_deref_prot(net->xfrm.state_bysrc, net);
	ospi = xfrm_state_deref_prot(net->xfrm.state_byspi, net);
	ohashmask = net->xfrm.state_hmask;

	rcu_assign_pointer(net->xfrm.state_bydst, ndst);
	rcu_assign_pointer(net->xfrm.state_bysrc, nsrc);
	rcu_assign_pointer(net->xfrm.state_byspi, nspi);
	net->xfrm.state_hmask = nhashmask;

	write_seqcount_end(&xfrm_state_hash_generation);
	spin_unlock_bh(&net->xfrm.xfrm_state_lock);

	osize = (ohashmask + 1) * sizeof(struct hlist_head);

	synchronize_rcu();

	xfrm_hash_free(odst, osize);
	xfrm_hash_free(osrc, osize);
	xfrm_hash_free(ospi, osize);
}