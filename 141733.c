static inline void xfrm_states_put(struct xfrm_state **states, int n)
{
	int i;
	for (i = 0; i < n; i++)
		xfrm_state_put(*(states + i));
}