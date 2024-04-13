static int jpc_dec_parseopts(char *optstr, jpc_dec_importopts_t *opts)
{
	jas_tvparser_t *tvp;

	opts->debug = 0;
	opts->maxlyrs = JPC_MAXLYRS;
	opts->maxpkts = -1;

	if (!(tvp = jas_tvparser_create(optstr ? optstr : ""))) {
		return -1;
	}

	while (!jas_tvparser_next(tvp)) {
		switch (jas_taginfo_nonull(jas_taginfos_lookup(decopts,
		  jas_tvparser_gettag(tvp)))->id) {
		case OPT_MAXLYRS:
			opts->maxlyrs = atoi(jas_tvparser_getval(tvp));
			break;
		case OPT_DEBUG:
			opts->debug = atoi(jas_tvparser_getval(tvp));
			break;
		case OPT_MAXPKTS:
			opts->maxpkts = atoi(jas_tvparser_getval(tvp));
			break;
		default:
			jas_eprintf("warning: ignoring invalid option %s\n",
			  jas_tvparser_gettag(tvp));
			break;
		}
	}

	jas_tvparser_destroy(tvp);

	return 0;
}