int xfrm_register_type_offload(const struct xfrm_type_offload *type,
			       unsigned short family)
{
	struct xfrm_state_afinfo *afinfo = xfrm_state_get_afinfo(family);
	const struct xfrm_type_offload **typemap;
	int err = 0;

	if (unlikely(afinfo == NULL))
		return -EAFNOSUPPORT;
	typemap = afinfo->type_offload_map;
	spin_lock_bh(&xfrm_type_offload_lock);

	if (likely(typemap[type->proto] == NULL))
		typemap[type->proto] = type;
	else
		err = -EEXIST;
	spin_unlock_bh(&xfrm_type_offload_lock);
	rcu_read_unlock();
	return err;
}