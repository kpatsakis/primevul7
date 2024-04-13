ntpq_custom_opt_handler(
	tOptions *pOptions,
	tOptDesc *pOptDesc
	)
{
	switch (pOptDesc->optValue) {

	default:
		fprintf(stderr,
			"ntpq_custom_opt_handler unexpected option '%c' (%d)\n",
			pOptDesc->optValue, pOptDesc->optValue);
		exit(1);

	case 'c':
		ADDCMD(pOptDesc->pzLastArg);
		break;

	case 'p':
		ADDCMD("peers");
		break;
	}
}