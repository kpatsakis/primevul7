authenticate(
	struct parse *pcmd,
	FILE *fp
	)
{
	if (pcmd->nargs == 0) {
		if (always_auth) {
			(void) fprintf(fp,
				       "authenticated requests being sent\n");
		} else
		    (void) fprintf(fp,
				   "unauthenticated requests being sent\n");
	} else {
		if (STREQ(pcmd->argval[0].string, "yes")) {
			always_auth = 1;
		} else if (STREQ(pcmd->argval[0].string, "no")) {
			always_auth = 0;
		} else
		    (void)fprintf(stderr, "What?\n");
	}
}