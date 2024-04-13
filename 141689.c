static void ___xfrm_state_destroy(struct xfrm_state *x)
{
	tasklet_hrtimer_cancel(&x->mtimer);
	del_timer_sync(&x->rtimer);
	kfree(x->aead);
	kfree(x->aalg);
	kfree(x->ealg);
	kfree(x->calg);
	kfree(x->encap);
	kfree(x->coaddr);
	kfree(x->replay_esn);
	kfree(x->preplay_esn);
	if (x->inner_mode)
		xfrm_put_mode(x->inner_mode);
	if (x->inner_mode_iaf)
		xfrm_put_mode(x->inner_mode_iaf);
	if (x->outer_mode)
		xfrm_put_mode(x->outer_mode);
	if (x->type_offload)
		xfrm_put_type_offload(x->type_offload);
	if (x->type) {
		x->type->destructor(x);
		xfrm_put_type(x->type);
	}
	xfrm_dev_state_free(x);
	security_xfrm_state_free(x);
	xfrm_state_free(x);
}