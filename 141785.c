static inline unsigned int xfrm_replay_state_esn_len(struct xfrm_replay_state_esn *replay_esn)
{
	return sizeof(*replay_esn) + replay_esn->bmp_len * sizeof(__u32);
}