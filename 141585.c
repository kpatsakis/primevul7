bool km_is_alive(const struct km_event *c)
{
	struct xfrm_mgr *km;
	bool is_alive = false;

	rcu_read_lock();
	list_for_each_entry_rcu(km, &xfrm_km_list, list) {
		if (km->is_alive && km->is_alive(c)) {
			is_alive = true;
			break;
		}
	}
	rcu_read_unlock();

	return is_alive;
}