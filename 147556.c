mpi_to_buffer(bigint_t a, byte * buf, size_t buflen,
	      size_t * r_nwritten, size_t * r_nbits)
{
	size_t nbits;
	int err;

	if (!a || !r_nwritten) {
		gnutls_assert();
		return CDK_Inv_Value;
	}

	nbits = _gnutls_mpi_get_nbits(a);
	if (r_nbits)
		*r_nbits = nbits;

	if (r_nwritten)
		*r_nwritten = (nbits + 7) / 8 + 2;

	if ((nbits + 7) / 8 + 2 > buflen)
		return CDK_Too_Short;

	*r_nwritten = buflen;
	err = _gnutls_mpi_print(a, buf, r_nwritten);
	if (err < 0) {
		gnutls_assert();
		return map_gnutls_error(err);
	}

	return 0;
}