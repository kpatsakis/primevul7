ntpversion(
	struct parse *pcmd,
	FILE *fp
	)
{
	if (pcmd->nargs == 0) {
		(void) fprintf(fp,
			       "NTP version being claimed is %d\n", pktversion);
	} else {
		if (pcmd->argval[0].uval < NTP_OLDVERSION
		    || pcmd->argval[0].uval > NTP_VERSION) {
			(void) fprintf(stderr, "versions %d to %d, please\n",
				       NTP_OLDVERSION, NTP_VERSION);
		} else {
			pktversion = (u_char) pcmd->argval[0].uval;
		}
	}
}