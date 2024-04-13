int km_migrate(const struct xfrm_selector *sel, u8 dir, u8 type,
	       const struct xfrm_migrate *m, int num_migrate,
	       const struct xfrm_kmaddress *k,
	       const struct xfrm_encap_tmpl *encap)
{
	int err = -EINVAL;
	int ret;
	struct xfrm_mgr *km;

	rcu_read_lock();
	list_for_each_entry_rcu(km, &xfrm_km_list, list) {
		if (km->migrate) {
			ret = km->migrate(sel, dir, type, m, num_migrate, k,
					  encap);
			if (!ret)
				err = ret;
		}
	}
	rcu_read_unlock();
	return err;
}