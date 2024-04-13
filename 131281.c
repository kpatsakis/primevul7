static unsigned char spk_ttyio_in_nowait(void)
{
	u8 rv = ttyio_in(0);

	return (rv == 0xff) ? 0 : rv;
}