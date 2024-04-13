int __xfrm_init_state(struct xfrm_state *x, bool init_replay, bool offload)
{
	struct xfrm_state_afinfo *afinfo;
	struct xfrm_mode *inner_mode;
	int family = x->props.family;
	int err;

	err = -EAFNOSUPPORT;
	afinfo = xfrm_state_get_afinfo(family);
	if (!afinfo)
		goto error;

	err = 0;
	if (afinfo->init_flags)
		err = afinfo->init_flags(x);

	rcu_read_unlock();

	if (err)
		goto error;

	err = -EPROTONOSUPPORT;

	if (x->sel.family != AF_UNSPEC) {
		inner_mode = xfrm_get_mode(x->props.mode, x->sel.family);
		if (inner_mode == NULL)
			goto error;

		if (!(inner_mode->flags & XFRM_MODE_FLAG_TUNNEL) &&
		    family != x->sel.family) {
			xfrm_put_mode(inner_mode);
			goto error;
		}

		x->inner_mode = inner_mode;
	} else {
		struct xfrm_mode *inner_mode_iaf;
		int iafamily = AF_INET;

		inner_mode = xfrm_get_mode(x->props.mode, x->props.family);
		if (inner_mode == NULL)
			goto error;

		if (!(inner_mode->flags & XFRM_MODE_FLAG_TUNNEL)) {
			xfrm_put_mode(inner_mode);
			goto error;
		}
		x->inner_mode = inner_mode;

		if (x->props.family == AF_INET)
			iafamily = AF_INET6;

		inner_mode_iaf = xfrm_get_mode(x->props.mode, iafamily);
		if (inner_mode_iaf) {
			if (inner_mode_iaf->flags & XFRM_MODE_FLAG_TUNNEL)
				x->inner_mode_iaf = inner_mode_iaf;
			else
				xfrm_put_mode(inner_mode_iaf);
		}
	}

	x->type = xfrm_get_type(x->id.proto, family);
	if (x->type == NULL)
		goto error;

	x->type_offload = xfrm_get_type_offload(x->id.proto, family, offload);

	err = x->type->init_state(x);
	if (err)
		goto error;

	x->outer_mode = xfrm_get_mode(x->props.mode, family);
	if (x->outer_mode == NULL) {
		err = -EPROTONOSUPPORT;
		goto error;
	}

	if (init_replay) {
		err = xfrm_init_replay(x);
		if (err)
			goto error;
	}

error:
	return err;
}