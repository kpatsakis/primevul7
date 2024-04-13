jpc_pi_t *jpc_pi_create0()
{
	jpc_pi_t *pi;
	if (!(pi = jas_malloc(sizeof(jpc_pi_t)))) {
		return 0;
	}
	pi->picomps = 0;
	pi->pchgno = 0;
	if (!(pi->pchglist = jpc_pchglist_create())) {
		jas_free(pi);
		return 0;
	}
	return pi;
}