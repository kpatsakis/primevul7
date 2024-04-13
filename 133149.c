int jpc_pi_init(jpc_pi_t *pi)
{
	int compno;
	int rlvlno;
	int prcno;
	jpc_picomp_t *picomp;
	jpc_pirlvl_t *pirlvl;
	int *prclyrno;

	pi->prgvolfirst = 0;
	pi->valid = 0;
	pi->pktno = -1;
	pi->pchgno = -1;
	pi->pchg = 0;

	for (compno = 0, picomp = pi->picomps; compno < pi->numcomps;
	  ++compno, ++picomp) {
		for (rlvlno = 0, pirlvl = picomp->pirlvls; rlvlno <
		  picomp->numrlvls; ++rlvlno, ++pirlvl) {
			for (prcno = 0, prclyrno = pirlvl->prclyrnos;
			  prcno < pirlvl->numprcs; ++prcno, ++prclyrno) {
				*prclyrno = 0;
			}
		}
	}
	return 0;
}