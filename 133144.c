static void jpc_picomp_destroy(jpc_picomp_t *picomp)
{
	int rlvlno;
	jpc_pirlvl_t *pirlvl;
	if (picomp->pirlvls) {
		for (rlvlno = 0, pirlvl = picomp->pirlvls; rlvlno <
		  picomp->numrlvls; ++rlvlno, ++pirlvl) {
			pirlvl_destroy(pirlvl);
		}
		jas_free(picomp->pirlvls);
	}
}