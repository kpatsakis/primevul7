doquery(
	int opcode,
	associd_t associd,
	int auth,
	int qsize,
	const char *qdata,
	u_short *rstatus,
	int *rsize,
	const char **rdata
	)
{
	return doqueryex(opcode, associd, auth, qsize, qdata, rstatus,
			 rsize, rdata, FALSE);
}