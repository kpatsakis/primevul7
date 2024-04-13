static jpc_fix_t jpc_calcabsstepsize(int stepsize, int numbits)
{
	jpc_fix_t absstepsize;
	int n;

	absstepsize = jpc_inttofix(1);
	n = JPC_FIX_FRACBITS - 11;
	absstepsize |= (n >= 0) ? (JPC_QCX_GETMANT(stepsize) << n) :
	  (JPC_QCX_GETMANT(stepsize) >> (-n));
	n = numbits - JPC_QCX_GETEXPN(stepsize);
	absstepsize = (n >= 0) ? (absstepsize << n) : (absstepsize >> (-n));
	return absstepsize;
}