u32 xfrm_get_acqseq(void)
{
	u32 res;
	static atomic_t acqseq;

	do {
		res = atomic_inc_return(&acqseq);
	} while (!res);

	return res;
}