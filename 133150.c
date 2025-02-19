void jpc_pi_destroy(jpc_pi_t *pi)
{
	jpc_picomp_t *picomp;
	int compno;
	if (pi->picomps) {
		for (compno = 0, picomp = pi->picomps; compno < pi->numcomps;
		  ++compno, ++picomp) {
			jpc_picomp_destroy(picomp);
		}
		jas_free(pi->picomps);
	}
	if (pi->pchglist) {
		jpc_pchglist_destroy(pi->pchglist);
	}
	jas_free(pi);
}