read_variables(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	if (res_associd)
		read_peervars();
	else
		read_sysvars();
}