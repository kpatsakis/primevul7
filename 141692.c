static inline int xfrm_replay_clone(struct xfrm_state *x,
				     struct xfrm_state *orig)
{
	x->replay_esn = kzalloc(xfrm_replay_state_esn_len(orig->replay_esn),
				GFP_KERNEL);
	if (!x->replay_esn)
		return -ENOMEM;

	x->replay_esn->bmp_len = orig->replay_esn->bmp_len;
	x->replay_esn->replay_window = orig->replay_esn->replay_window;

	x->preplay_esn = kmemdup(x->replay_esn,
				 xfrm_replay_state_esn_len(x->replay_esn),
				 GFP_KERNEL);
	if (!x->preplay_esn) {
		kfree(x->replay_esn);
		return -ENOMEM;
	}

	return 0;
}