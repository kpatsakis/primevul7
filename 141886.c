xfrm_get_type_offload(u8 proto, unsigned short family, bool try_load)
{
	struct xfrm_state_afinfo *afinfo;
	const struct xfrm_type_offload **typemap;
	const struct xfrm_type_offload *type;

retry:
	afinfo = xfrm_state_get_afinfo(family);
	if (unlikely(afinfo == NULL))
		return NULL;
	typemap = afinfo->type_offload_map;

	type = typemap[proto];
	if ((type && !try_module_get(type->owner)))
		type = NULL;

	rcu_read_unlock();

	if (!type && try_load) {
		request_module("xfrm-offload-%d-%d", family, proto);
		try_load = false;
		goto retry;
	}

	return type;
}