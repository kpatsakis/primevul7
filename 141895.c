static inline int xfrm_policy_id2dir(u32 index)
{
	return index & 7;
}