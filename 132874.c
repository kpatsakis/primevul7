static void jpc_dec_cp_destroy(jpc_dec_cp_t *cp)
{
	if (cp->ccps) {
		jas_free(cp->ccps);
	}
	if (cp->pchglist) {
		jpc_pchglist_destroy(cp->pchglist);
	}
	jas_free(cp);
}