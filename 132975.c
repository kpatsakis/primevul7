crypto_update_taichange(void)
{
	static const u_int len = 3 * sizeof(u_int32);

	/* check if the signing digest algo is available */
	if (sign_digest == NULL || sign_pkey == NULL)
		return;

	/* check size of TAI extension block */
	if (tai_leap.ptr == NULL || ntohl(tai_leap.vallen) != len)
		return;

	/* crypto_update should at least not crash here! */
	crypto_update();
}