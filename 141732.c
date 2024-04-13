static inline void xfrm_states_delete(struct xfrm_state **states, int n)
{
	int i;
	for (i = 0; i < n; i++)
		xfrm_state_delete(*(states + i));
}