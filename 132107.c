static void fips_expand_key_bits(BYTE* in, BYTE* out)
{
	BYTE buf[21], c;
	int i, b, p, r;

	/* reverse every byte in the key */
	for (i = 0; i < 21; i++)
		buf[i] = fips_reverse_table[in[i]];

	/* insert a zero-bit after every 7th bit */
	for (i = 0, b = 0; i < 24; i++, b += 7)
	{
		p = b / 8;
		r = b % 8;

		if (r <= 1)
		{
			out[i] = (buf[p] << r) & 0xfe;
		}
		else
		{
			/* c is accumulator */
			c = buf[p] << r;
			c |= buf[p + 1] >> (8 - r);
			out[i] = c & 0xfe;
		}
	}

	/* reverse every byte */
	/* alter lsb so the byte has odd parity */
	for (i = 0; i < 24; i++)
		out[i] = fips_oddparity_table[fips_reverse_table[out[i]]];
}